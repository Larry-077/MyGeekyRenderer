#include <iostream>
#include <chrono>
#include <cstring> // for strncpy
#include <cmath>
#include <algorithm>
#include <Eigen/Core>

#ifdef USE_IMGUI
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif

#include "Scene.h"
#include "Camera.h"
#include "ASCIIRenderer.h"
#include "CameraController.h"

// ==========================================
// 全局变量
// ==========================================
Scene g_scene;
Camera g_camera;
ASCIIRenderer g_renderer;
CameraController g_camera_controller;

// 用于 UI 输入的文件路径缓存
char g_model_path_buffer[256] = ""; 
// 光源旋转角度 (0-360)
float g_light_angle = 45.0f;

double g_fps = 0.0;
double g_render_time = 0.0;

// ==========================================
// 模型加载函数
// ==========================================
void load_model(const std::string& filename) {
    if (filename.empty()) return;

    std::cout << "Loading: " << filename << std::endl;
    
    // 重置场景
    g_scene = Scene(); 
    g_scene.load_mesh(filename);
    
    if (g_scene.bvh) {
        Eigen::RowVector3d center = g_scene.bvh->box.center();
        Eigen::RowVector3d size = g_scene.bvh->box.max_corner - g_scene.bvh->box.min_corner;
        double max_size = size.maxCoeff();
        
        // 自动适配相机
        g_camera_controller.set_target_and_fit(
            Eigen::Vector3d(center(0), center(1), center(2)),
            max_size * 0.8 // 稍微拉近一点，让物体显示更大
        );
        
        std::cout << "✓ Loaded: " << g_scene.bvh->num_leaves << " triangles" << std::endl;
    } else {
        std::cerr << "Failed to load model or model is empty." << std::endl;
    }
}

#ifdef USE_IMGUI

// [已删除] 鼠标交互相关的回调函数和结构体

// ==========================================
// 主运行函数 (ImGui 循环)
// ==========================================
void run_with_imgui() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    
    // MacOS 兼容设置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1400, 900, "ASCII 3D Renderer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启垂直同步
    
    // [已删除] glfwSetMouseButtonCallback 和 glfwSetCursorPosCallback
    
    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    
    // 如果启动时有参数，先加载一下
    if (strlen(g_model_path_buffer) > 0) {
        load_model(g_model_path_buffer);
    }
    
    // 初始化默认值
    g_renderer.resolution = 120;
    g_camera_controller.scale = 1.0; 
    
    // 初始化光照：确保环境光不要太强，否则看不出光照方向变化
    g_renderer.ambient_strength = 0.2; 
    g_renderer.light.intensity = 1.0;

    auto last_time = std::chrono::high_resolution_clock::now();
    
    // --- 主循环 ---
    while (!glfwWindowShouldClose(window)) {
        // 1. 时间计算
        auto current_time = std::chrono::high_resolution_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - last_time).count();
        last_time = current_time;
        g_fps = 1.0 / delta_time;
        
        // 2. 更新逻辑
        
        // 2.1 更新相机 (仅处理自动旋转)
        g_camera_controller.update(delta_time);
        g_camera_controller.apply_to_camera(g_camera);
        
        // 2.2 [关键修复] 更新光照
        // Y 改为 -0.5 (向下照)，而不是 1.0 (向上照)
        double angle_rad = g_light_angle * M_PI / 180.0;
        g_renderer.light.direction = Eigen::Vector3d(sin(angle_rad), -0.5, cos(angle_rad)).normalized();
        
        // 3. 渲染 ASCII
        auto render_start = std::chrono::high_resolution_clock::now();
        std::string ascii_frame = g_renderer.render(g_scene, g_camera);
        auto render_end = std::chrono::high_resolution_clock::now();
        g_render_time = std::chrono::duration<double>(render_end - render_start).count();
        
        // 4. ImGui 绘制
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // --- 窗口 1: ASCII 显示区 ---
        const float ascii_window_width = 1100.0f;
        const float ascii_window_height = 880.0f;
        
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ascii_window_width, ascii_window_height), ImGuiCond_Always);
        ImGui::Begin("ASCII View", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        
        int grid_w, grid_h;
        g_renderer.get_grid_size(grid_w, grid_h);
        float base_char_width = 7.0f; 
        float desired_char_width = ascii_window_width / grid_w;
        float font_scale = desired_char_width / base_char_width;
        
        ImGui::SetWindowFontScale(font_scale);
        ImGui::TextUnformatted(ascii_frame.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::End();
        
        // --- 窗口 2: 控制面板 ---
        ImGui::SetNextWindowPos(ImVec2(1120, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(270, 880), ImGuiCond_Always);
        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        ImGui::Text("FPS: %.0f", g_fps);
        ImGui::Text("Render: %.1f ms", g_render_time * 1000.0);
        
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        
        // 模型加载器
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Model Selection");
        const char* model_names[] = { "Mitsuba Sphere", "Dragon", "Buddha"};
        const char* model_paths[] = { "../assets/mitsuba-sphere.obj", "../assets/dragon.obj", "../assets/buddha.obj"};
        static int current_model_idx = -1;
        
        if (ImGui::Combo("Presets", &current_model_idx, model_names, IM_ARRAYSIZE(model_names))) {
            strncpy(g_model_path_buffer, model_paths[current_model_idx], sizeof(g_model_path_buffer) - 1);
            load_model(g_model_path_buffer);
        }
        
        ImGui::Text("Custom Path:");
        ImGui::InputText("##path", g_model_path_buffer, IM_ARRAYSIZE(g_model_path_buffer));
        if (ImGui::Button("Load", ImVec2(-1, 0))) {
            load_model(g_model_path_buffer);
        }
        
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        
        // 渲染设置
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Render Settings");
        ImGui::Text("Resolution");
        ImGui::SliderInt("##res", &g_renderer.resolution, 40, 400);
        
        ImGui::Text("Scale");
        float scale_val = (float)g_camera_controller.scale;
        if (ImGui::SliderFloat("##scale", &scale_val, 0.1f, 10.0f)) {
            g_camera_controller.set_scale(scale_val);
        }
        
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        
        // 光照设置
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Lighting");
        ImGui::Text("Light Angle");
        ImGui::SliderFloat("##angle", &g_light_angle, 0.0f, 360.0f);
        
        ImGui::Text("Intensity");
        float intensity = (float)g_renderer.light.intensity;
        if (ImGui::SliderFloat("##intensity", &intensity, 0.0f, 2.0f)) {
            g_renderer.light.intensity = intensity;
        }
        
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        
        ImGui::Checkbox("Auto Rotate", &g_camera_controller.auto_rotate);
        if (ImGui::Button("Reset View", ImVec2(-1, 0))) {
            g_camera_controller.theta = M_PI / 2.0;
            g_camera_controller.phi = 0.0;
            g_camera_controller.scale = 1.0;
            g_light_angle = 45.0f;
            g_camera_controller.auto_rotate = true;
        }
        
        ImGui::End();
        
        // 5. 渲染
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
    
    // 清理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

#endif

int main(int argc, char* argv[]) {
    if (argc > 1) {
        strncpy(g_model_path_buffer, argv[1], sizeof(g_model_path_buffer) - 1);
    } 
    
    #ifdef USE_IMGUI
        run_with_imgui();
    #else
        std::cerr << "Please compile with -DUSE_IMGUI=ON" << std::endl;
        return 1;
    #endif
    
    return 0;
}