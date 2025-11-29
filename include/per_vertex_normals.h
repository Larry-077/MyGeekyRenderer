#ifndef PER_VERTEX_NORMALS_H
#define PER_VERTEX_NORMALS_H

#include <Eigen/Core>

// Compute per-vertex normals for a triangle mesh.
//
// Inputs:
//   V  #V by 3 matrix of vertex positions
//   F  #F by 3 matrix of face indices
// Outputs:
//   N  #V by 3 matrix of vertex normals
void per_vertex_normals(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  Eigen::MatrixXd & N);

#endif
