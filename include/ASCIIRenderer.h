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
    // 渲染参数
    int resolution;           // 横向字符数
    double scale;             // 相机缩放
    bool invert_brightness;   // 反转亮度（黑底白字 vs 白底黑字）
    int charset_type;         // 字符集类型 0=简单 1=详细 2=块状
    
    // 光照参数
    DirectionalLight light;
    double ambient_strength;  // 环境光强度
    
    // 字符集
    std::vector<std::string> charsets;
    
    ASCIIRenderer() 
        : resolution(80)
        , scale(1.0)
        , invert_brightness(false)
        , charset_type(1)
        , ambient_strength(0.2)
    {
        // 初始化字符集
        charsets.push_back(" .:-=+*#%@");  // 简单
        charsets.push_back(" .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$");  // 详细
        charsets.push_back(" ░▒▓█");  // 块状字符
        
        // 默认光源（从右上方照射）
        light = DirectionalLight(Eigen::Vector3d(1, 1, -1).normalized(), 1.0);
    }
    
    // 渲染主函数
    std::string render(const Scene& scene, const Camera& camera);
    
    // 计算单条射线的字符
    char trace_ray(const Scene& scene, const Ray& ray, const std::string& charset);
    
    // 计算亮度
    double calculate_brightness(const Eigen::Vector3d& normal, const Eigen::Vector3d& view_dir);
    
    // 亮度映射到字符
    char brightness_to_char(double brightness, const std::string& charset);
    
    // 获取网格尺寸
    void get_grid_size(int& width, int& height) const {
        width = resolution;
        height = resolution / 2;  // ASCII 字符高度约为宽度的2倍
    }
};

#endif
