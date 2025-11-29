#ifndef MESH_TRIANGLE_H
#define MESH_TRIANGLE_H

#include "Object.h"
#include <Eigen/Core>
#include <memory>

struct MeshTriangle : public Object
{
  public:
    // Pointer to mesh vertex position list
    const Eigen::MatrixXd & V;
    // Pointer to mesh indices list
    const Eigen::MatrixXi & F;
    // Pointer to vertex normals (optional, can be nullptr)
    const Eigen::MatrixXd * N;
    // face index
    int f;
    
    // Record with vertex and face set this MeshTriangle references _and_
    // compute bounding box for this triangle.
    //
    // Inputs:
    //   V  pointer to mesh vertex list
    //   F  pointer to mesh face list
    //   f  index of triangle in _F
    //   N  optional pointer to vertex normals
    // Side effects: inserts this triangle into .box (see Object.h)
    inline MeshTriangle(
      const Eigen::MatrixXd & V,
      const Eigen::MatrixXi & F,
      const int f,
      const Eigen::MatrixXd * N = nullptr);
      
    // Get normal at a point (can use face normal or interpolated vertex normals)
    inline Eigen::Vector3d get_normal(const Eigen::Vector3d & p) const;
    
    // Object implementations (see Object.h)
    inline bool intersect(
      const Ray & ray, 
      const double min_t, 
      double & t, 
      Eigen::Vector3d & n) const override;
      
    inline bool ray_intersect(
      const Ray& ray,
      const double min_t,
      const double max_t,
      double & t,
      std::shared_ptr<Object> & descendant) const override;
      
    inline bool point_squared_distance(
      const Eigen::RowVector3d & query,
      const double min_sqrd,
      const double max_sqrd,
      double & sqrd,
      std::shared_ptr<Object> & descendant) const override
    {
      // Not implemented for this assignment
      assert(false && "point_squared_distance not implemented for MeshTriangle");
      return false;
    }
};


// Implementation

#include "insert_triangle_into_box.h"
#include "ray_intersect_triangle.h"

inline MeshTriangle::MeshTriangle(
    const Eigen::MatrixXd & _V,
    const Eigen::MatrixXi & _F,
    const int _f,
    const Eigen::MatrixXd * _N): V(_V), F(_F), f(_f), N(_N)
{
  insert_triangle_into_box(
    V.row(F(f,0)),
    V.row(F(f,1)),
    V.row(F(f,2)),
    box);
}

// Get normal at point p
inline Eigen::Vector3d MeshTriangle::get_normal(const Eigen::Vector3d & p) const
{
  // If we have vertex normals, we could interpolate them using barycentric coordinates
  // For now, just return face normal (simpler and sufficient for most cases)
  Eigen::RowVector3d v0 = V.row(F(f,0));
  Eigen::RowVector3d v1 = V.row(F(f,1));
  Eigen::RowVector3d v2 = V.row(F(f,2));
  
  Eigen::RowVector3d edge1 = v1 - v0;
  Eigen::RowVector3d edge2 = v2 - v0;
  
  // Cross product for RowVector3d
  Eigen::RowVector3d normal;
  normal(0) = edge1(1) * edge2(2) - edge1(2) * edge2(1);
  normal(1) = edge1(2) * edge2(0) - edge1(0) * edge2(2);
  normal(2) = edge1(0) * edge2(1) - edge1(1) * edge2(0);
  
  return normal.normalized().transpose();
}

// Simple wrapper around `ray_intersect_triangle`
inline bool MeshTriangle::ray_intersect(
  const Ray& ray,
  const double min_t,
  const double max_t,
  double & t,
  std::shared_ptr<Object> & descendant) const
{
  bool hit = ray_intersect_triangle(
    ray,
    V.row(F(f,0)),
    V.row(F(f,1)),
    V.row(F(f,2)),
    min_t,
    max_t,
    t);
  
  if (hit) {
    // Set descendant to nullptr - this is a leaf object
    descendant = nullptr;
  }
  
  return hit;
}

// Implementation of intersect (for compatibility with old Object interface)
inline bool MeshTriangle::intersect(
  const Ray & ray, 
  const double min_t, 
  double & t, 
  Eigen::Vector3d & n) const
{
  bool hit = ray_intersect_triangle(
    ray,
    V.row(F(f,0)),
    V.row(F(f,1)),
    V.row(F(f,2)),
    min_t,
    std::numeric_limits<double>::infinity(),
    t);
  
  if (hit) {
    n = get_normal(ray.origin + t * ray.direction);
  }
  
  return hit;
}

#endif
