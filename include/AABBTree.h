#ifndef AABBTREE_H
#define AABBTREE_H

#include "BoundingBox.h"
#include "Object.h"
#include <Eigen/Core>
#include <memory>
#include <vector>
#include <cassert>

// Implementation
#include "ray_intersect_box.h"

struct AABBTree : public Object, public std::enable_shared_from_this<AABBTree>
{
  // Pointers to left and right subtree branches. These could be another
  // AABBTree (internal node) or a leaf (primitive Object like MeshTriangle, or
  // CloudPoint)
  std::shared_ptr<Object> left;
  std::shared_ptr<Object> right;
  // For debugging, keep track of the depth (root has depth == 0)
  int depth;
  // For debugging, keep track of the number leaf, descendants 
  int num_leaves;
  // Construct a axis-aligned bounding box tree given a list of objects. Use the
  // midpoint along the longest axis of the box containing the given objects to
  // determine the left-right split.
  //
  // Inputs:
  //   objects  list of objects to store in this AABBTree
  //   Optional inputs:
  //     depth  depth of this tree (usually set by constructor of parent as
  //       their depth+1)
  // Side effects: num_leaves is set to objects.size() and left/right pointers
  // set to subtrees or leaf Objects accordingly.
  AABBTree(
    const std::vector<std::shared_ptr<Object> > & objects, 
    int depth=0);
  // Object implementations (see Object.h for API)
  bool intersect(
    const Ray & ray, 
    const double min_t, 
    double & t, 
    Eigen::Vector3d & n) const override
  {
    // Simple wrapper around ray_intersect
    std::shared_ptr<Object> descendant;
    bool hit = ray_intersect(ray, min_t, std::numeric_limits<double>::infinity(), t, descendant);
    if (hit && descendant) {
      // Get normal from the descendant object
      descendant->intersect(ray, min_t, t, n);
    }
    return hit;
  }
  
  bool ray_intersect(
    const Ray& ray,
    const double min_t,
    const double max_t,
    double & t,
    std::shared_ptr<Object> & descendant) const override;
  bool point_squared_distance(
    const Eigen::RowVector3d & query,
    const double min_sqrd,
    const double max_sqrd,
    double & sqrd,
    std::shared_ptr<Object> & descendant) const override
  {
    assert(false && "Do not use recursive DFS for AABBTree distance");
    return false;
  }
};

#endif
