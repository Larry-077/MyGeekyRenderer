#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Core>

struct DirectionalLight {
    Eigen::Vector3d direction;  
    double intensity;          
    
    DirectionalLight() 
        : direction(0, 0, -1), intensity(1.0) {}
    
    DirectionalLight(const Eigen::Vector3d& dir, double intens = 1.0)
        : direction(dir.normalized()), intensity(intens) {}
};

struct PointLight {
    Eigen::Vector3d position;
    double intensity;
    
    PointLight()
        : position(0, 10, 0), intensity(1.0) {}
    
    PointLight(const Eigen::Vector3d& pos, double intens = 1.0)
        : position(pos), intensity(intens) {}
};

#endif
