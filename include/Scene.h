#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <Eigen/Core>

#include "Object.h"
#include "MeshTriangle.h"
#include "AABBTree.h"
#include "read_obj.h"
#include "per_vertex_normals.h"

struct Scene {
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::MatrixXd N; 

    std::vector<std::shared_ptr<Object>> objects;

    std::shared_ptr<AABBTree> bvh;

    void load_mesh(const std::string& filename) {
        if (!read_obj(filename, V, F)) {
            std::cerr << "Failed to load obj!" << std::endl;
            return;
        }

        per_vertex_normals(V, F, N);

        objects.clear();
        for (int i = 0; i < F.rows(); ++i) {
            auto tri = std::make_shared<MeshTriangle>(V, F, i, &N);
            objects.push_back(tri);
        }

        bvh = std::make_shared<AABBTree>(objects);
        std::cout << "BVH Built. Leaves: " << objects.size() << std::endl;
    }

    bool intersect(const Ray& ray, double min_t, double max_t, 
                   double& t, Eigen::Vector3d& n, 
                   std::shared_ptr<Object>& hit_obj) const 
    {
        if (!bvh) return false;
        
        if (bvh->ray_intersect(ray, min_t, max_t, t, hit_obj)) {
            auto tri = std::dynamic_pointer_cast<MeshTriangle>(hit_obj);
            if (tri) {
            
                Eigen::Vector3d p = ray.origin + t * ray.direction;
                n = tri->get_normal(p); 
                return true;
            }
            return false;
        }
        return false;
    }
};

#endif
