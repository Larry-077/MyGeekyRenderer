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
    // 1. 原始数据 (必须一直存活)
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::MatrixXd N; // 顶点法线

    // 2. 物体列表 (用于构建 BVH)
    std::vector<std::shared_ptr<Object>> objects;

    // 3. 加速结构 (BVH 根节点)
    std::shared_ptr<AABBTree> bvh;

    // 加载函数
    void load_mesh(const std::string& filename) {
        // 读取
        if (!read_obj(filename, V, F)) {
            std::cerr << "Failed to load obj!" << std::endl;
            return;
        }

        // 计算平滑法线 (为了 Toon Shading 效果好)
        per_vertex_normals(V, F, N);

        // 创建 MeshTriangle 对象
        objects.clear();
        for (int i = 0; i < F.rows(); ++i) {
            auto tri = std::make_shared<MeshTriangle>(V, F, i, &N);
            objects.push_back(tri);
        }

        // 构建 BVH
        bvh = std::make_shared<AABBTree>(objects);
        std::cout << "BVH Built. Leaves: " << objects.size() << std::endl;
    }

    // 统一求交接口
    bool intersect(const Ray& ray, double min_t, double max_t, 
                   double& t, Eigen::Vector3d& n, 
                   std::shared_ptr<Object>& hit_obj) const 
    {
        if (!bvh) return false;
        
        if (bvh->ray_intersect(ray, min_t, max_t, t, hit_obj)) {
            // 击中后，强制转换为 MeshTriangle 来获取法线
            auto tri = std::dynamic_pointer_cast<MeshTriangle>(hit_obj);
            if (tri) {
                // 计算击中点
                Eigen::Vector3d p = ray.origin + t * ray.direction;
                n = tri->get_normal(p); 
                return true;
            }
            // 如果不是 MeshTriangle，可能是 BVH 内部节点，需要继续遍历
            // 但正常情况下 hit_obj 应该是叶子节点
            return false;
        }
        return false;
    }
};

#endif
