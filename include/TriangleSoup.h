#ifndef TRIANGLE_SOUP_H
#define TRIANGLE_SOUP_H

#include "Object.h"
#include "Triangle.h"
#include <vector>
#include <memory>

class TriangleSoup : public Object
{
  public:
    std::vector<std::shared_ptr<Triangle>> triangles;
    
    bool intersect(
      const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const override;
      
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
        descendant = nullptr;
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
      assert(false && "point_squared_distance not implemented for TriangleSoup");
      return false;
    }
};

#endif
