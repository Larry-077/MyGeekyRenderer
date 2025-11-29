#include <iostream>
#include <chrono>
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

// 全局变量
Scene g_scene;
Camera g_camera;
ASCIIRenderer g_renderer;
CameraController g_camera_controller;

double g_fps = 0.0;
double g_render_time = 0.0;

void load_model(const std::string& filename) {
    std::cout << "Loading: " << filename << std::endl;
    g_scene.load_mesh(filename);
    
    if (g_scene.bvh) {
        Eigen::RowVector3d center = g_scene.bvh->box.center();
        Eigen::RowVector3d size = g_scene.bvh->box.max_corner - g_scene.bvh->box.min_corner;
        double max_size = size.maxCoeff();
        
        g_camera_controller.set_target_and_fit(
            Eigen::Vector3d(center(0), center(1), center(2)),
            max_size
        );
        
        std::cout << "✓ Loaded: " << g_scene.bvh->num_leaves << " triangles" << std::endl;
    }
}

#ifdef USE_IMGUI

// 鼠标状态
struct MouseState {
    bool dragging = false;
    double last_x = 0.0;
    double last_y = 0.0;
} g_mouse;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // 忽略 ImGui 窗口上的点击
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            g_mouse.dragging = true;
            glfwGetCursorPos(window, &g_mouse.last_x, &g_mouse.last_y);
        } else if (action == GLFW_RELEASE) {
            g_mouse.dragging = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_mouse.dragging) {
        double dx = xpos - g_mouse.last_x;
        double dy = ypos - g_mouse.last_y;
        
        // 暂停自动旋转
        g_camera_controller.auto_rotate = false;
        
        // 更新角度
        g_camera_controller.phi -= dx * 0.005;
        g_camera_controller.theta += dy * 0.005;
        
        // 限制俯仰角
        g_camera_controller.theta = std::clamp(g_camera_controller.theta, 0.1, M_PI - 0.1);
        
        g_mouse.last_x = xpos;
        g_mouse.last_y = ypos;
    }
}

void run_with_imgui(const std::string& model_path) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    
    const char* glsl_version = "#version 150";
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
    glfwSwapInterval(1);
    
    // 设置回调
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    
    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    load_model(model_path);
    
    // 主循环
    auto last_time = std::chrono::high_resolution_clock::now();
    
    while (!glfwWindowShouldClose(window)) {
        auto current_time = std::chrono::high_resolution_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - last_time).count();
        last_time = current_time;
        
        g_fps = 1.0 / delta_time;
        
        // 更新相机
        g_camera_controller.update(delta_time);
        g_camera_controller.apply_to_camera(g_camera);
        
        // 渲染 ASCII
        auto render_start = std::chrono::high_resolution_clock::now();
        std::string ascii_frame = g_renderer.render(g_scene, g_camera);
        auto render_end = std::chrono::high_resolution_clock::now();
        g_render_time = std::chrono::duration<double>(render_end - render_start).count();
        
        // ImGui
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // ASCII 显示窗口
        const float ascii_window_width = 1100.0f;
        const float ascii_window_height = 880.0f;
        
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ascii_window_width, ascii_window_height), ImGuiCond_Always);
        ImGui::Begin("ASCII View", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        
        // 关键！根据 Resolution 自动调整字符大小
        int grid_w, grid_h;
        g_renderer.get_grid_size(grid_w, grid_h);
        
        // 计算字体缩放，使字符填满窗口
        float base_char_width = 8.0f;  // 假设基础字符宽度 8px
        float desired_char_width = ascii_window_width / grid_w;
        float font_scale = desired_char_width / base_char_width;
        
        ImGui::SetWindowFontScale(font_scale);
        ImGui::TextUnformatted(ascii_frame.c_str());
        ImGui::SetWindowFontScale(1.0f);
        
        ImGui::End();
        
        // 控制面板
        ImGui::SetNextWindowPos(ImVec2(1120, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(270, 880), ImGuiCond_Always);
        ImGui::Begin("Controls", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        // 性能
        ImGui::Text("FPS: %.0f", g_fps);
        ImGui::Text("Render: %.1f ms", g_render_time * 1000.0);
        ImGui::Text("Rays: %d", grid_w * grid_h);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Resolution - 控制网格密度和字符大小
        ImGui::Text("Resolution");
        ImGui::SliderInt("##res", &g_renderer.resolution, 20, 200, "%d");
        ImGui::TextWrapped("Controls character grid density and size");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Scale - 控制视野（相机 width/height）
        ImGui::Text("Scale");
        float scale_val = (float)g_camera_controller.scale;
        if (ImGui::SliderFloat("##scale", &scale_val, 0.3f, 3.0f, "%.2f")) {
            g_camera_controller.set_scale(scale_val);
        }
        ImGui::TextWrapped("Controls zoom (field of view)");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // 光照
        ImGui::Text("Lighting");
        ImGui::SliderFloat("##light", (float*)&g_renderer.light.intensity, 0.0f, 2.0f, "%.1f");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // 交互说明
        ImGui::TextWrapped("Mouse Controls:");
        ImGui::BulletText("Drag to rotate");
        ImGui::Spacing();
        
        ImGui::Checkbox("Auto Rotate", &g_camera_controller.auto_rotate);
        
        ImGui::Spacing();
        
        if (ImGui::Button("Reset View", ImVec2(-1, 0))) {
            g_camera_controller.theta = M_PI / 2.0;
            g_camera_controller.phi = 0.0;
            g_camera_controller.scale = 1.0;
            g_camera_controller.auto_rotate = true;
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // 技术信息
        ImGui::TextWrapped("Resolution: %dx%d grid", grid_w, grid_h);
        ImGui::TextWrapped("Font scale: %.2f", font_scale);
        ImGui::TextWrapped("Camera radius: %.1f (fixed)", g_camera_controller.radius);
        ImGui::TextWrapped("View size: %.2f", g_camera_controller.base_view_size / g_camera_controller.scale);
        
        ImGui::End();
        
        // 渲染
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
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <model.obj>" << std::endl;
        return 1;
    }
    
    #ifdef USE_IMGUI
        run_with_imgui(argv[1]);
    #else
        std::cerr << "Please compile with -DUSE_IMGUI=ON" << std::endl;
        return 1;
    #endif
    
    return 0;
}