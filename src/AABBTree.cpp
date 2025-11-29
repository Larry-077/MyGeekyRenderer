#include "AABBTree.h"
#include "insert_box_into_box.h"
#include <algorithm>

AABBTree::AABBTree(
  const std::vector<std::shared_ptr<Object> > & objects,
  int a_depth)
: depth(a_depth),
num_leaves(objects.size())
{
  this->box.min_corner = Eigen::RowVector3d(
      std::numeric_limits<double>::infinity(),
      std::numeric_limits<double>::infinity(),
      std::numeric_limits<double>::infinity()
  );
  this->box.max_corner = Eigen::RowVector3d(
      -std::numeric_limits<double>::infinity(),
      -std::numeric_limits<double>::infinity(),
      -std::numeric_limits<double>::infinity()
  );
  
  for (const auto & obj : objects) {
      insert_box_into_box(obj->box, this->box);
  }

  if (objects.size() == 1) {
      left = objects[0];
      right = nullptr;
      return;
  }

  Eigen::RowVector3d diag = this->box.max_corner - this->box.min_corner;
  int axis;
  diag.maxCoeff(&axis);

  double mid = this->box.center()[axis];

  std::vector<std::shared_ptr<Object>> left_objs;
  std::vector<std::shared_ptr<Object>> right_objs;

  for (const auto & obj : objects) {
      double center = obj->box.center()[axis];
      if (center <= mid)
          left_objs.push_back(obj);
      else
          right_objs.push_back(obj);
  }

  if (left_objs.empty()) {
      left_objs.assign(objects.begin(), objects.begin() + objects.size() / 2);
      right_objs.assign(objects.begin() + objects.size() / 2, objects.end());
  } else if (right_objs.empty()) {
      left_objs.assign(objects.begin(), objects.begin() + objects.size() / 2);
      right_objs.assign(objects.begin() + objects.size() / 2, objects.end());
  }

  left  = std::make_shared<AABBTree>(left_objs,  depth + 1);
  right = std::make_shared<AABBTree>(right_objs, depth + 1);
}