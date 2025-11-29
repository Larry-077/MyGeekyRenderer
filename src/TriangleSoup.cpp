#include "TriangleSoup.h"
#include "Ray.h"
// Hint
#include "first_hit.h"

bool TriangleSoup::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  // Convert vector<shared_ptr<Triangle>> to vector<shared_ptr<Object>>
  std::vector<std::shared_ptr<Object>> objects;
  objects.reserve(triangles.size());
  for (const auto& tri : triangles) {
    objects.push_back(tri);
  }
  
  int hit_id;
  return first_hit(ray, min_t, objects, hit_id, t, n);
}



