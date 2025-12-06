#include <iostream>
#include <chrono>
#include <cstring>
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

Scene g_scene;
Camera g_camera;
ASCIIRenderer g_renderer;
CameraController g_camera_controller;

char g_model_path_buffer[256] = ""; 
float g_light_theta = 120.0f;
float g_light_phi =150.0f;

double g_fps = 0.0;
double g_render_time = 0.0;

void load_model(const std::string& filename) {
    if (filename.empty()) return;

    std::cout << "Loading: " << filename << std::endl;
    
    g_scene = Scene(); 
    g_scene.load_mesh(filename);
    
    if (g_scene.bvh) {
        Eigen::RowVector3d center = g_scene.bvh->box.center();
        Eigen::RowVector3d size = g_scene.bvh->box.max_corner - g_scene.bvh->box.min_corner;
        double max_size = size.maxCoeff();
        
        g_camera_controller.set_target_and_fit(
            Eigen::Vector3d(center(0), center(1), center(2)),
            max_size * 0.8
        );
        
        std::cout << "✓ Loaded: " << g_scene.bvh->num_leaves << " triangles" << std::endl;
    } else {
        std::cerr << "Failed to load model or model is empty." << std::endl;
    }
}

#ifdef USE_IMGUI

void run_with_imgui() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    
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
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    
    if (strlen(g_model_path_buffer) > 0) {
        load_model(g_model_path_buffer);
    }
    
    g_renderer.resolution = 120;
    g_camera_controller.scale = 1.0; 
    g_renderer.ambient_strength = 0.2; 
    g_renderer.light.intensity = 0.7;

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Render();
    glfwSwapBuffers(window);
    
    ImVec2 char_size = ImGui::CalcTextSize("@");
    float actual_char_aspect = char_size.y / char_size.x;
    g_renderer.aspect_ratio_correction = actual_char_aspect / 2.0;
    
    std::cout << "Font character size: " << char_size.x << " x " << char_size.y << std::endl;
    std::cout << "Character aspect ratio (H/W): " << actual_char_aspect << std::endl;
    std::cout << "Aspect correction factor: " << g_renderer.aspect_ratio_correction << std::endl;

    auto last_time = std::chrono::high_resolution_clock::now();
    
    while (!glfwWindowShouldClose(window)) {
        auto current_time = std::chrono::high_resolution_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - last_time).count();
        last_time = current_time;
        g_fps = 1.0 / delta_time;
        
        g_camera_controller.update(delta_time);
        g_camera_controller.apply_to_camera(g_camera, g_renderer.aspect_ratio_correction);
        
        double theta_rad = g_light_theta * M_PI / 180.0;
        double phi_rad = g_light_phi * M_PI / 180.0;
        
        Eigen::Vector3d camera_right = g_camera.u;
        Eigen::Vector3d camera_up = g_camera.v;
        Eigen::Vector3d camera_forward = -g_camera.w;
        
        g_renderer.light.direction = (
            sin(theta_rad) * cos(phi_rad) * camera_right +
            cos(theta_rad) * camera_up +
            sin(theta_rad) * sin(phi_rad) * camera_forward
        ).normalized();
        
        auto render_start = std::chrono::high_resolution_clock::now();
        std::string ascii_frame = g_renderer.render(g_scene, g_camera);
        auto render_end = std::chrono::high_resolution_clock::now();
        g_render_time = std::chrono::duration<double>(render_end - render_start).count();
        
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
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
        
        ImGui::SetNextWindowPos(ImVec2(1120, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(270, 880), ImGuiCond_Always);
        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        ImGui::Text("FPS: %.0f", g_fps);
        ImGui::Text("Render: %.1f ms", g_render_time * 1000.0);
        
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        
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
        
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Render Settings");
        ImGui::Text("Resolution");
        ImGui::SliderInt("##res", &g_renderer.resolution, 40, 400);
        
        ImGui::Text("Scale");
        float scale_val = (float)g_camera_controller.scale;
        if (ImGui::SliderFloat("##scale", &scale_val, 0.1f, 10.0f)) {
            g_camera_controller.set_scale(scale_val);
        }
        
        ImGui::Text("Charset");
        const char* charset_names[] = {"Simple", "Detailed"};
        ImGui::Combo("##charset", &g_renderer.charset_type, charset_names, 2);
        
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Lighting");
        ImGui::Text("Light Type: Directional + Ambient");
        ImGui::Text("(Camera Space - Follows View)");
        
        ImGui::Text("Light Theta (Elevation)");
        ImGui::SliderFloat("##theta", &g_light_theta, 0.0f, 180.0f);
        
        ImGui::Text("Light Phi (Azimuth)");
        ImGui::SliderFloat("##phi", &g_light_phi, 0.0f, 360.0f);
        
        ImGui::Text("Light Intensity");
        float intensity = (float)g_renderer.light.intensity;
        if (ImGui::SliderFloat("##intensity", &intensity, 0.0f, 2.0f)) {
            g_renderer.light.intensity = intensity;
        }
        
        ImGui::Text("Ambient Strength");
        float ambient = (float)g_renderer.ambient_strength;
        if (ImGui::SliderFloat("##ambient", &ambient, 0.0f, 1.0f)) {
            g_renderer.ambient_strength = ambient;
        }
        
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        
        ImGui::Checkbox("Auto Rotate", &g_camera_controller.auto_rotate);
        if (ImGui::Button("Reset View", ImVec2(-1, 0))) {
            g_camera_controller.theta = M_PI / 2.0;
            g_camera_controller.phi = 0.0;
            g_camera_controller.scale = 1.0;
            g_light_theta = 120.0f;
            g_light_phi = 90.0f;
            g_camera_controller.auto_rotate = true;
        }
        
        ImGui::End();
        
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
    
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