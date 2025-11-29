#include "ray_intersect_box.h"
#include <algorithm>   
#include <cmath>      
#include <limits>     

bool ray_intersect_box(
  const Ray & ray,
  const BoundingBox& box,
  const double min_t,
  const double max_t)
{
  double tmin = -std::numeric_limits<double>::infinity();
  double tmax =  std::numeric_limits<double>::infinity();

  for (int i = 0; i < 3; i++) {
    double origin = ray.origin[i];
    double direction = ray.direction[i];
    double min_c = box.min_corner[i];
    double max_c = box.max_corner[i];

    if (std::abs(direction) < 1e-8) {
      if (origin < min_c || origin > max_c)
        return false;
    } else {
      double t1 = (min_c - origin) / direction;
      double t2 = (max_c - origin) / direction;
      if (t1 > t2) std::swap(t1, t2);

      tmin = std::max(tmin, t1);
      tmax = std::min(tmax, t2);

      if (tmin > tmax)
        return false;
    }
  }

  return (tmax >= min_t) && (tmin <= max_t);
}

