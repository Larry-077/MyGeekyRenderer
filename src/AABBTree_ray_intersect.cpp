#include "AABBTree.h"

// See AABBTree.h for API
bool AABBTree::ray_intersect(
    const Ray& ray,
    const double min_t,
    const double max_t,
    double & t,
    std::shared_ptr<Object> & descendant) const
  {
    if (!ray_intersect_box(ray, this->box, min_t, max_t))
        return false;
  
    // 如果这是叶子节点（只有left，没有right）
    if (left && !right)
    {
        // 直接调用叶子对象的 ray_intersect
        bool hit = left->ray_intersect(ray, min_t, max_t, t, descendant);
        // 确保 descendant 被正确设置
        if (hit && !descendant) {
            descendant = left;
        }
        return hit;
    }
  
    // 如果没有子节点
    if (!left && !right)
        return false;
  
    bool hit_left  = false, hit_right = false;
    double t_left  = std::numeric_limits<double>::infinity();
    double t_right = std::numeric_limits<double>::infinity();
    std::shared_ptr<Object> obj_left, obj_right;
  
    if (left)
        hit_left = left->ray_intersect(ray, min_t, max_t, t_left, obj_left);
  
    if (right)
        hit_right = right->ray_intersect(ray, min_t, max_t, t_right, obj_right);
  
    if (hit_left && hit_right)
    {
        if (t_left < t_right) { 
            t = t_left; 
            descendant = obj_left; 
        }
        else {
            t = t_right; 
            descendant = obj_right; 
        }
        return true;
    }
    else if (hit_left)
    {
        t = t_left; 
        descendant = obj_left; 
        return true;
    }
    else if (hit_right)
    {
        t = t_right; 
        descendant = obj_right; 
        return true;
    }
  
    return false;
  }