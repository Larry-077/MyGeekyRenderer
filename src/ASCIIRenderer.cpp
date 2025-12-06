#include "ASCIIRenderer.h"
#include <algorithm>
#include <cmath>


std::string ASCIIRenderer::render(const Scene& scene, const Camera& camera) {
    std::string output;
    
    double angle_rad = light_angle * M_PI / 180.0;
    light.direction = Eigen::Vector3d(sin(angle_rad), 1.0, cos(angle_rad)).normalized();
    
    int grid_width, grid_height;
    get_grid_size(grid_width, grid_height);
    
    const std::string& charset = charsets[charset_type];
    
    // 预分配字符串大小
    output.reserve(grid_height * (grid_width + 1));
    
    // 遍历每个字符位置
    for (int row = 0; row < grid_height; row++) {
        for (int col = 0; col < grid_width; col++) {
            // 生成射线
            Ray ray;
            viewing_ray(camera, row, col, grid_width, grid_height, ray);
            
            // 追踪并获取字符
            char c = trace_ray(scene, ray, charset);
            output += c;
        }
        output += '\n';
    }
    
    return output;
}

char ASCIIRenderer::trace_ray(const Scene& scene, const Ray& ray, const std::string& charset) {
    double t;
    Eigen::Vector3d n;
    std::shared_ptr<Object> hit_obj;
    
    // 射线求交
    if (scene.intersect(ray, 0.01, std::numeric_limits<double>::infinity(), t, n, hit_obj)) {
        // 计算亮度
        double brightness = calculate_brightness(n, ray.direction);
        
        // 映射到字符
        return brightness_to_char(brightness, charset);
    }
    
    // 背景
    return ' ';
}

double ASCIIRenderer::calculate_brightness(const Eigen::Vector3d& normal, const Eigen::Vector3d& view_dir) {
    // Diffuse lighting (Lambertian)
    double diffuse = std::max(0.0, normal.dot(-light.direction)) * light.intensity;
    
    // Ambient lighting
    double ambient = ambient_strength;
    
    // 组合
    double brightness = std::min(1.0, ambient + diffuse);
    
    // 边缘光（rim light）- 可选
    double rim = std::pow(1.0 - std::abs(normal.dot(-view_dir)), 2.0);
    brightness += rim * 0.1;  // 轻微的边缘高光
    
    brightness = std::clamp(brightness, 0.0, 1.0);
    
    return brightness;
}

char ASCIIRenderer::brightness_to_char(double brightness, const std::string& charset) {
    if (invert_brightness) {
        brightness = 1.0 - brightness;
    }
    
    int index = static_cast<int>(brightness * (charset.length() - 1));
    index = std::clamp(index, 0, static_cast<int>(charset.length() - 1));
    
    return charset[index];
}
