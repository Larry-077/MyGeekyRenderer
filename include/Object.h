#ifndef OBJECT_H
#define OBJECT_H

#include <Eigen/Core>
#include <memory>
#include "BoundingBox.h"

struct Ray;
class Object
{
  public:
    // Bounding box for this object
    BoundingBox box;
    
    // https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    virtual ~Object() {}
    
    // Intersect object with ray.
    //
    // Inputs:
    //   Ray  ray to intersect with
    //   min_t  minimum parametric distance to consider
    // Outputs:
    //   t  first intersection at ray.origin + t * ray.direction
    //   n  surface normal at point of intersection
    // Returns iff there a first intersection is found.
    //
    // The funny = 0 just ensures that this function is defined (as a no-op)
    virtual bool intersect(
        const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const = 0;
    
    // New interface for AABBTree and MeshTriangle
    // Inputs:
    //   ray  ray to intersect with
    //   min_t  minimum parametric distance to consider
    //   max_t  maximum parametric distance to consider
    // Outputs:
    //   t  first intersection at ray.origin + t * ray.direction
    //   descendant  the actual object that was hit (for BVH traversal)
    // Returns true iff there is an intersection
    virtual bool ray_intersect(
        const Ray& ray,
        const double min_t,
        const double max_t,
        double & t,
        std::shared_ptr<Object> & descendant) const = 0;
    
    // Point-object squared distance query
    // Inputs:
    //   query  query point
    //   min_sqrd  minimum squared distance to consider
    //   max_sqrd  maximum squared distance to consider
    // Outputs:
    //   sqrd  squared distance to object
    //   descendant  the actual object that was closest (for BVH traversal)
    // Returns true iff there is a point within the distance range
    virtual bool point_squared_distance(
        const Eigen::RowVector3d & query,
        const double min_sqrd,
        const double max_sqrd,
        double & sqrd,
        std::shared_ptr<Object> & descendant) const = 0;
};

#endif
