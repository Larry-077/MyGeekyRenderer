#include "read_obj.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

bool read_obj(
  const std::string & filename,
  Eigen::MatrixXd & V,
  Eigen::MatrixXi & F)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open file " << filename << std::endl;
    return false;
  }

  std::vector<Eigen::RowVector3d> v_list;
  std::vector<Eigen::RowVector3i> f_list;

  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string type;
    ss >> type;

    if (type == "v") {
      double x, y, z;
      ss >> x >> y >> z;
      v_list.emplace_back(x, y, z);
    } 
    else if (type == "f") {
      std::string v_str;
      std::vector<int> face_indices;
      
      while (ss >> v_str) {
        size_t slash_pos = v_str.find('/');
        int v_idx = std::stoi(v_str.substr(0, slash_pos));
        if (v_idx < 0) {
          v_idx = v_list.size() + v_idx;
        } else {
          v_idx = v_idx - 1;
        }
        
        face_indices.push_back(v_idx);
      }

      if (face_indices.size() == 3) {
        f_list.emplace_back(face_indices[0], face_indices[1], face_indices[2]);
      } else if (face_indices.size() == 4) {
        f_list.emplace_back(face_indices[0], face_indices[1], face_indices[2]);
        f_list.emplace_back(face_indices[0], face_indices[2], face_indices[3]);
      }
    }
  }

  V.resize(v_list.size(), 3);
  for (size_t i = 0; i < v_list.size(); i++) V.row(i) = v_list[i];

  F.resize(f_list.size(), 3);
  for (size_t i = 0; i < f_list.size(); i++) F.row(i) = f_list[i];

  return true;
}