#include "ASCIIRenderer.h"
#include <algorithm>
#include <cmath>

std::string ASCIIRenderer::render(const Scene& scene, const Camera& camera) {
    std::string output;
    
    int grid_width, grid_height;
    get_grid_size(grid_width, grid_height);
    
    const std::string& charset = charsets[charset_type];
    
    output.reserve(grid_height * (grid_width + 1));
    
    for (int row = 0; row < grid_height; row++) {
        for (int col = 0; col < grid_width; col++) {
            Ray ray;
            viewing_ray(camera, row, col, grid_width, grid_height, ray);
            
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
    
    if (scene.intersect(ray, 0.01, std::numeric_limits<double>::infinity(), t, n, hit_obj)) {
        double brightness = calculate_brightness(n, ray.direction);
        return brightness_to_char(brightness, charset);
    }
    
    return ' ';
}

double ASCIIRenderer::calculate_brightness(const Eigen::Vector3d& normal, const Eigen::Vector3d& view_dir) {
    double diffuse = std::max(0.0, normal.dot(-light.direction)) * light.intensity;
    double ambient = ambient_strength;
    double brightness = std::clamp(ambient + diffuse, 0.0, 1.0);
    
    return brightness;
}

char ASCIIRenderer::brightness_to_char(double brightness, const std::string& charset) {
    int index = static_cast<int>(brightness * (charset.length() - 1));
    index = std::clamp(index, 0, static_cast<int>(charset.length() - 1));
    
    return charset[index];
}