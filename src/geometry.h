#pragma once

#include "glm/glm.hpp"
#include "mesh.h"

namespace MR {

struct AABB {
    glm::vec3 min, max;

    void build(const Mesh &mesh);
    void update(const glm::mat4 &modelMat);
};

// Equation is Ax + By + Cz + D = 0
struct Plane {
    glm::vec3 normal;
    float D;

    float distance(const glm::vec3 &points);
    void set_normal_and_point(const glm::vec3 &normal, const glm::vec3 &point);
};

// The shape of the camera view area, looks like an inverse pyramid with the top
// missing
struct Frustum {
    enum planes { TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP };

    Plane pl[6];
    float fov, near_plane, far_plane, ar, near_h, near_w;

    void set_cam_internals();
    void update_planes(glm::mat4 &view_mat, const glm::vec3 &camera_pos);
    bool check_if_inside(AABB *bounds);
};

}  // namespace MR