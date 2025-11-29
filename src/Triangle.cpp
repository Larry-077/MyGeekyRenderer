#include "Triangle.h"
#include "Ray.h"
#include <Eigen/Dense>

bool Triangle::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  const Eigen::Vector3d& v0 = std::get<0>(corners);
  Eigen::Vector3d x1 = std::get<1>(corners) - v0;
  Eigen::Vector3d x2 = std::get<2>(corners) - v0;
  Eigen::Matrix3d A;
  A.col(0) = x1;
  A.col(1) = x2;
  A.col(2) = -ray.direction;
  double det = A.determinant();
  double epsilon = 1e-6;
  if(std::abs(det) < epsilon){
    return false;
  }
  Eigen::Vector3d x = A.inverse() * (ray.origin - v0);
  double t1 = x(0);
  double t2 = x(1);
  double t0 = x(2);
  if(t1 >= 0 && t2 >= 0 && t1 + t2 <= 1 && t0 >= min_t){
    t = t0;
    n = x1.cross(x2).normalized();
    return true;
  } else{
    return false;
  }
}


