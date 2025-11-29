#include "per_vertex_normals.h"
#include "triangle_area_normal.h"
#include "vertex_triangle_adjacency.h"
#include <vector>

void per_vertex_normals(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  Eigen::MatrixXd & N)
{
  N = Eigen::MatrixXd::Zero(V.rows(), 3);

  Eigen::MatrixXd FN;
  FN.resize(F.rows(), 3);
  for(int i = 0; i < F.rows(); i++)
  {
    FN.row(i) = triangle_area_normal(
      V.row(F(i,0)), V.row(F(i,1)), V.row(F(i,2)));
  }

  std::vector<std::vector<int>> VF;
  vertex_triangle_adjacency(F, V.rows(), VF);

  for (int i = 0; i < V.rows(); ++i)
  {
    Eigen::RowVector3d sum_normal(0, 0, 0);

    const std::vector<int>& adjacent_faces = VF[i];

    for (int face_index : adjacent_faces)
    {
      sum_normal += FN.row(face_index);
    }

    if (sum_normal.norm() > 1e-10)
    {
      N.row(i) = sum_normal.normalized();
    }
  }
}