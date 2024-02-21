#pragma once

#include "glm/glm.hpp"
#include "mesh.h"

namespace MR {

struct AABB {
    glm::vec3 min, max;

    void build(const Mesh& mesh);
    void update(const glm::mat4& modelMat);
};

struct Plane {
    glm::vec3 normal;
    float D;

    float distance(const glm::vec3& points);
    void set_normal_and_point(const glm::vec3& normal, const glm::vec3& point);
};

struct Frustum {
    enum planes { TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP };

    Plane pl[6];
    float fov, near_plane, far_plane, ar, near_h, near_w;

    void set_cam_internals();
    void update_planes(glm::mat4& view_mat, const glm::vec3& camera_pos);
    bool check_if_inside(const AABB& bounds);
};

}  // namespace MR