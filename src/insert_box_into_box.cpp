#include "insert_box_into_box.h"

void insert_box_into_box(
  const BoundingBox & A,
  BoundingBox & B)
{
  for (int i = 0; i < 3; i++) {
    B.min_corner[i] = std::min(B.min_corner[i], A.min_corner[i]);
    B.max_corner[i] = std::max(B.max_corner[i], A.max_corner[i]);
  }
}

