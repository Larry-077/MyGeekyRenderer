#ifndef RAY_INTERSECT_TRIANGLE_H
#define RAY_INTERSECT_TRIANGLE_H

#include "Ray.h"
#include <Eigen/Core>

// Intersect a ray with a triangle.
//
// Inputs:
//   ray  ray to intersect with
//   A  first corner of triangle
//   B  second corner of triangle
//   C  third corner of triangle
//   min_t  minimum parametric distance to consider
//   max_t  maximum parametric distance to consider
// Outputs:
//   t  parametric distance of intersection
// Returns true iff there is an intersection
bool ray_intersect_triangle(
  const Ray & ray,
  const Eigen::RowVector3d & A,
  const Eigen::RowVector3d & B,
  const Eigen::RowVector3d & C,
  const double min_t,
  const double max_t,
  double & t);

#endif
