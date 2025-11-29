#include "ray_intersect_triangle.h"
#include <Eigen/Dense>
#include <cmath>

bool ray_intersect_triangle(
  const Ray & ray,
  const Eigen::RowVector3d & A,
  const Eigen::RowVector3d & B,
  const Eigen::RowVector3d & C,
  const double min_t,
  const double max_t,
  double & t)
{
  Eigen::Vector3d e1 = (B - A).transpose();
  Eigen::Vector3d e2 = (C - A).transpose();

  Eigen::Matrix3d M;
  M.col(0) = e1;
  M.col(1) = e2;
  M.col(2) = -ray.direction;

  double det = M.determinant();
  const double epsilon = 1e-8;

  if (std::abs(det) < epsilon)
  {
    return false;
  }

  Eigen::Vector3d rhs = ray.origin - A.transpose();
  Eigen::Vector3d x = M.inverse() * rhs;

  double u = x(0);
  double v = x(1);
  double tt = x(2);

  if (u >= 0 && v >= 0 && u + v <= 1 && tt >= min_t && tt <= max_t)
  {
    t = tt;
    return true;
  }

  return false;
}
