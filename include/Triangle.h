#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.h"
#include <Eigen/Core>

class Triangle : public Object
{
  public:
    // A triangle has three corners
    std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> corners;
    
    // Intersect a triangle with ray.
    //
    // Inputs:
    //   Ray  ray to intersect with
    //   min_t  minimum parametric distance to consider
    // Outputs:
    //   t  first intersection at ray.origin + t * ray.direction
    //   n  surface normal at point of intersection
    // Returns iff there a first intersection is found.
    bool intersect(
      const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const override;
    
    // ray_intersect implementation for compatibility with AABBTree
    bool ray_intersect(
      const Ray& ray,
      const double min_t,
      const double max_t,
      double & t,
      std::shared_ptr<Object> & descendant) const override
    {
      Eigen::Vector3d n;
      bool hit = intersect(ray, min_t, t, n);
      if (hit && t <= max_t) {
        descendant = nullptr; // Triangle is a leaf object
        return true;
      }
      return false;
    }
    
    bool point_squared_distance(
      const Eigen::RowVector3d & query,
      const double min_sqrd,
      const double max_sqrd,
      double & sqrd,
      std::shared_ptr<Object> & descendant) const override
    {
      assert(false && "point_squared_distance not implemented for Triangle");
      return false;
    }
};

#endif
