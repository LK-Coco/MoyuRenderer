#include "geometry.h"

namespace MR {

void AABB::build(const Mesh &mesh) {
    glm::vec3 min_vals(std::numeric_limits<float>::max());
    glm::vec3 max_vals(std::numeric_limits<float>::min());

    for (int i = 0; i < mesh.vertices.size(); ++i) {
        max_vals.x = std::max(max_vals.x, mesh.vertices[i].position.x);
        max_vals.y = std::max(max_vals.y, mesh.vertices[i].position.y);
        max_vals.z = std::max(max_vals.z, mesh.vertices[i].position.z);

        min_vals.x = std::min(min_vals.x, mesh.vertices[i].position.x);
        min_vals.y = std::min(min_vals.y, mesh.vertices[i].position.y);
        min_vals.z = std::min(min_vals.z, mesh.vertices[i].position.z);
    }
    min = min_vals;
    max = max_vals;
}

void AABB::update(const glm::mat4 &modelMat) {}

float Plane::distance(const glm::vec3 &points) {
    return glm::dot(normal, points) + D;
}

void Plane::set_normal_and_point(const glm::vec3 &n, const glm::vec3 &point) {
    normal = n;
    D = -glm::dot(normal, point);
}

void Frustum::set_cam_internals() {
    float tan_half_fov = (float)tan(glm::radians(fov / 2.0f));
    near_h = near_plane * tan_half_fov;
    near_w = near_h * ar;
}

void Frustum::update_planes(glm::mat4 &view_mat, const glm::vec3 &camera_pos) {
    set_cam_internals();
    glm::vec3 X(view_mat[0][0], view_mat[0][1],
                view_mat[0][2]);  // Side Unit Vector
    glm::vec3 Y(view_mat[1][0], view_mat[1][1],
                view_mat[1][2]);  // Up Unit Vector
    glm::vec3 Z(view_mat[2][0], view_mat[2][1],
                view_mat[2][2]);  // Forward vector

    // Gets worlds space position of the center points of the near and far
    // planes The forward vector Z points towards the viewer so you need to
    // negate it and scale it by the distance (near or far) to the plane to get
    // the center positions
    glm::vec3 near_center = camera_pos - Z * near_plane;
    glm::vec3 far_center = camera_pos - Z * far_plane;

    glm::vec3 point;
    glm::vec3 normal;

    // We build the planes using a normal and a point (in this case the center)
    // Z is negative here because we want the normal vectors we choose to point
    // towards the inside of the view frustrum that way we can cehck in or out
    // with a simple Dot product
    pl[NEARP].set_normal_and_point(-Z, near_center);
    // Far plane
    pl[FARP].set_normal_and_point(Z, far_center);

    // Again, want to get the plane from a normal and point
    // You scale the up vector by the near plane height and added to the
    // nearcenter to optain a point on the edge of both near and top plane.
    // Subtracting the cameraposition from this point generates a vector that
    // goes along the surface of the plane, if you take the cross product with
    // the direction vector equal to the shared edge of the planes you get the
    // normal
    point = near_center + Y * near_h;
    normal = glm::normalize(point - camera_pos);
    normal = glm::cross(normal, X);
    pl[TOP].set_normal_and_point(normal, point);

    // Bottom plane
    point = near_center - Y * near_h;
    normal = glm::normalize(point - camera_pos);
    normal = glm::cross(X, normal);
    pl[BOTTOM].set_normal_and_point(normal, point);

    // Left plane
    point = near_center - X * near_w;
    normal = glm::normalize(point - camera_pos);
    normal = glm::cross(normal, Y);
    pl[LEFT].set_normal_and_point(normal, point);

    // Right plane
    point = near_center + X * near_w;
    normal = glm::normalize(point - camera_pos);
    normal = glm::cross(Y, normal);
    pl[RIGHT].set_normal_and_point(normal, point);
}

bool Frustum::check_if_inside(AABB *box) {
    for (int i = 0; i < 6; ++i) {
        int out = 0;
        out += ((pl[i].distance(glm::vec3(box->min.x, box->min.y, box->min.z)) <
                 0.0)
                    ? 1
                    : 0);
        out += ((pl[i].distance(glm::vec3(box->max.x, box->min.y, box->min.z)) <
                 0.0)
                    ? 1
                    : 0);
        out += ((pl[i].distance(glm::vec3(box->min.x, box->max.y, box->min.z)) <
                 0.0)
                    ? 1
                    : 0);
        out += ((pl[i].distance(glm::vec3(box->max.x, box->max.y, box->min.z)) <
                 0.0)
                    ? 1
                    : 0);
        out += ((pl[i].distance(glm::vec3(box->min.x, box->min.y, box->max.z)) <
                 0.0)
                    ? 1
                    : 0);
        out += ((pl[i].distance(glm::vec3(box->max.x, box->min.y, box->max.z)) <
                 0.0)
                    ? 1
                    : 0);
        out += ((pl[i].distance(glm::vec3(box->min.x, box->max.y, box->max.z)) <
                 0.0)
                    ? 1
                    : 0);
        out += ((pl[i].distance(glm::vec3(box->max.x, box->max.y, box->max.z)) <
                 0.0)
                    ? 1
                    : 0);

        if (out == 8) return false;
    }
    return true;
}

}  // namespace MR