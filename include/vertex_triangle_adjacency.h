#ifndef VERTEX_TRIANGLE_ADJACENCY_H
#define VERTEX_TRIANGLE_ADJACENCY_H

#include <Eigen/Core>
#include <vector>

// Compute vertex-triangle adjacency list.
//
// Inputs:
//   F  #F by 3 matrix of face indices
//   num_vertices  number of vertices
// Outputs:
//   VF  list of lists so that VF[i] is a list of face indices incident on vertex i
void vertex_triangle_adjacency(
  const Eigen::MatrixXi & F,
  const int num_vertices,
  std::vector<std::vector<int> > & VF);

#endif
