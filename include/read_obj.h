#ifndef READ_OBJ_H
#define READ_OBJ_H

#include <Eigen/Core>
#include <string>
#include <vector>

// Inputs:
//   filename  
// Outputs:
//   V  Vertices (n x 3 matrix)
//   F  Faces (m x 3 matrix)
// Returns true if successful
bool read_obj(
  const std::string & filename,
  Eigen::MatrixXd & V,
  Eigen::MatrixXi & F);

#endif