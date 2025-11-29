#ifndef RAY_INTERSECT_BOX_H
#define RAY_INTERSECT_BOX_H

#include "Ray.h"
#include "BoundingBox.h"

// Intersect a ray with an axis-aligned bounding box.
//
// Inputs:
//   ray  ray to intersect with
//   box  axis-aligned bounding box
//   min_t  minimum parametric distance to consider
//   max_t  maximum parametric distance to consider
// Returns true iff there is an intersection
bool ray_intersect_box(
  const Ray & ray,
  const BoundingBox& box,
  const double min_t,
  const double max_t);

#endif
