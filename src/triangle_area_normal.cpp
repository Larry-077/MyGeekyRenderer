#include "triangle_area_normal.h"

Eigen::RowVector3d triangle_area_normal(
  const Eigen::RowVector3d & a, 
  const Eigen::RowVector3d & b, 
  const Eigen::RowVector3d & c)
{
  // Compute two edges (keep as RowVector3d)
  Eigen::RowVector3d edge1 = b - a;
  Eigen::RowVector3d edge2 = c - a;
  
  // Manual cross product for RowVector3d
  Eigen::RowVector3d area_normal;
  area_normal(0) = edge1(1) * edge2(2) - edge1(2) * edge2(1);
  area_normal(1) = edge1(2) * edge2(0) - edge1(0) * edge2(2);
  area_normal(2) = edge1(0) * edge2(1) - edge1(1) * edge2(0);
  
  return area_normal;
}
