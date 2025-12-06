#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <algorithm>
#include <cmath>
#include "Camera.h"

class CameraController {
public:
    // 固定球面参数
    double radius;              // 相机到物体的距离（固定！）
    double theta;               // 极角（俯仰）
    double phi;                 // 方位角（旋转）
    Eigen::Vector3d target;     // 物体中心
    
    // 自动旋转
    bool auto_rotate;
    double rotation_speed;      // 度/秒
    
    // 视野控制（Scale 改变这个！）
    double base_view_size;      // 基础视野大小
    double scale;               // 缩放因子 (0.5 - 3.0)
    
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
    
    void apply_to_camera(Camera& camera) {
        // 球坐标 → 笛卡尔坐标（radius 固定！）
        camera.e.x() = target.x() + radius * sin(theta) * cos(phi);
        camera.e.y() = target.y() + radius * cos(theta);
        camera.e.z() = target.z() + radius * sin(theta) * sin(phi);
        
        // 计算视线方向
        Eigen::Vector3d view_dir = (target - camera.e).normalized();
        camera.w = -view_dir;
        
        // 构建正交基
        Eigen::Vector3d up(0, 1, 0);
        if (std::abs(view_dir.dot(up)) > 0.99) {
            up = Eigen::Vector3d(0, 0, 1);
        }
        
        // 手动叉积
        camera.u(0) = up(1) * camera.w(2) - up(2) * camera.w(1);
        camera.u(1) = up(2) * camera.w(0) - up(0) * camera.w(2);
        camera.u(2) = up(0) * camera.w(1) - up(1) * camera.w(0);
        camera.u.normalize();
        
        camera.v(0) = camera.w(1) * camera.u(2) - camera.w(2) * camera.u(1);
        camera.v(1) = camera.w(2) * camera.u(0) - camera.w(0) * camera.u(2);
        camera.v(2) = camera.w(0) * camera.u(1) - camera.w(1) * camera.u(0);
        
        // 关键！Scale 改变视野大小，不改变 radius
        camera.d = 1.0;
        double current_view_size = base_view_size / scale;  // scale 越大，视野越小（放大）
        camera.width = current_view_size;
        camera.height = current_view_size;  // 先设为正方形，后面可能需要调整
    }
    
    void set_scale(double s) {
        scale = std::clamp(s, 0.01, 100.0);
    }
    
    void set_target_and_fit(const Eigen::Vector3d& center, double size) {
        target = center;
        radius = size * 2.0;           // 固定距离
        base_view_size = size * 1.0;   // 基础视野
        scale = 1.0;                   // 重置缩放
    }
};

#endif