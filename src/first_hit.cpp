#include "first_hit.h"
#include <limits>

bool first_hit(
  const Ray & ray, 
  const double min_t,
  const std::vector< std::shared_ptr<Object> > & objects,
  int & hit_id, 
  double & t,
  Eigen::Vector3d & n)
{
  double closest_t = std::numeric_limits<double>::infinity();
  bool found_hit = false;
  
  for (int i = 0; i < objects.size(); i++)
  {
    double temp_t;
    Eigen::Vector3d temp_n;
    
    if (objects[i]->intersect(ray, min_t, temp_t, temp_n))
    {
      if (temp_t < closest_t)
      {
        closest_t = temp_t;
        hit_id = i;
        n = temp_n;
        found_hit = true;
      }
    }
  }
  
  if (found_hit)
  {
    t = closest_t;
    return true;
  }
  
  return false;
}
