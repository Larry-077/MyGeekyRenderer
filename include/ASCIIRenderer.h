#ifndef ASCII_RENDERER_H
#define ASCII_RENDERER_H

#include <string>
#include <vector>
#include <Eigen/Core>
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Ray.h"
#include "viewing_ray.h"

class ASCIIRenderer {
public:
    int resolution;
    double ambient_strength;
    DirectionalLight light;
    
    int charset_type;
    std::vector<std::string> charsets;
    double aspect_ratio_correction;
    
    ASCIIRenderer() 
        : resolution(80)
        , ambient_strength(0.2)
        , charset_type(0)
        , aspect_ratio_correction(1.0)
    {
        charsets.push_back(" .:-=+*#%@");
        charsets.push_back(" .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$");
        
        light = DirectionalLight(Eigen::Vector3d(1, 1, -1).normalized(), 1.0);
    }
    
    std::string render(const Scene& scene, const Camera& camera);
    char trace_ray(const Scene& scene, const Ray& ray, const std::string& charset);
    double calculate_brightness(const Eigen::Vector3d& normal, const Eigen::Vector3d& view_dir);
    char brightness_to_char(double brightness, const std::string& charset);
    
    void get_grid_size(int& width, int& height) const {
        width = resolution;
        height = resolution / 2;
    }
};

#endif