#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <algorithm>
#include <cmath>
#include "Camera.h"

class CameraController {
public:
    double radius;              
    double theta;               
    double phi;                 
    Eigen::Vector3d target;     
    
    bool auto_rotate;
    double rotation_speed;     
    
    double base_view_size;     
    double scale;               
    
    CameraController()
        : radius(10.0)
        , theta(M_PI / 2.0)
        , phi(0.0)
        , target(0, 0, 0)
        , auto_rotate(true)
        , rotation_speed(30.0)
        , base_view_size(5.0)
        , scale(1.0)
    {}
    
    void update(double delta_time) {
        if (auto_rotate) {
            phi += rotation_speed * delta_time * M_PI / 180.0;
            if (phi > 2.0 * M_PI) phi -= 2.0 * M_PI;
        }
    }
    
    void apply_to_camera(Camera& camera, double aspect_correction = 1.0) {
        camera.e.x() = target.x() + radius * sin(theta) * cos(phi);
        camera.e.y() = target.y() + radius * cos(theta);
        camera.e.z() = target.z() + radius * sin(theta) * sin(phi);
        
        Eigen::Vector3d view_dir = (target - camera.e).normalized();
        camera.w = -view_dir;
        
        Eigen::Vector3d up(0, 1, 0);
        if (std::abs(view_dir.dot(up)) > 0.99) {
            up = Eigen::Vector3d(0, 0, 1);
        }
        
        camera.u(0) = up(1) * camera.w(2) - up(2) * camera.w(1);
        camera.u(1) = up(2) * camera.w(0) - up(0) * camera.w(2);
        camera.u(2) = up(0) * camera.w(1) - up(1) * camera.w(0);
        camera.u.normalize();
        
        camera.v(0) = camera.w(1) * camera.u(2) - camera.w(2) * camera.u(1);
        camera.v(1) = camera.w(2) * camera.u(0) - camera.w(0) * camera.u(2);
        camera.v(2) = camera.w(0) * camera.u(1) - camera.w(1) * camera.u(0);
        
        camera.d = 1.0;
        double current_view_size = base_view_size / scale;
        camera.width = current_view_size;
        camera.height = current_view_size * aspect_correction;  // ← 使用修正系数
    }
    
    void set_scale(double s) {
        scale = std::clamp(s, 0.01, 100.0);
    }
    
    void set_target_and_fit(const Eigen::Vector3d& center, double size) {
        target = center;
        radius = size * 2.0;           
        base_view_size = size * 1.0;  
        scale = 1.0;                
    }
};

#endif