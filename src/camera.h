#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "geometry.h"

namespace MR {

constexpr glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);
constexpr float MovementSpeed = 5.0f;

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
public:
    Camera(glm::vec3 pos, float yaw = -90.0f, float pitch = 0.0f)
        : position(pos),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          yaw(yaw),
          pitch(pitch) {
        update_front();
    }

    Camera(glm::vec3 pos, glm::vec3 tar, float fov, float ar, float near_p,
           float far_p) {
        position = pos;
        target = tar;
        front_ = glm::normalize(target - position);
        right_ = glm::normalize(glm::cross(front_, WORLD_UP));
        pitch = calc_pitch(front_);
        yaw = calc_yaw(front_, pitch);

        frustum.fov = fov;
        frustum.ar = ar;
        frustum.near_plane = near_p;
        frustum.far_plane = far_p;

        projection_mat_ =
            glm::perspective(glm::radians(frustum.fov), frustum.ar,
                             frustum.near_plane, frustum.far_plane);
        view_mat_ = glm::lookAt(position, target, up_);
        frustum.update_planes(view_mat_, position);
    }

    bool check_visibility(const AABB& bounds) {
        return frustum.check_if_inside(bounds);
    }

    glm::mat4 get_projection() const { return projection_mat_; }

    glm::mat4 get_view_mat() const { return view_mat_; }

    float get_zoom() const { return zoom_; }

    void on_process_mouse_scroll(float scroll) {
        zoom_ -= (float)scroll;
        if (zoom_ < 1.0f) zoom_ = 1.0f;
        if (zoom_ > 45.0f) zoom_ = 45.0f;
    }

    void on_process_mouse_move(float x_offset, float y_offset) {
        // TODO 重构：四元数的旋转
        yaw += x_offset * 0.5f;
        pitch += y_offset * 0.5f;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
        if (yaw > 360.f) yaw -= 360.f;
        if (yaw < -360.f) yaw += 360.f;

        update_front();
        update_mat4();
    }

    void on_process_keyboard(CameraMovement direction, float delta_time) {
        float velocity = MovementSpeed * delta_time;
        if (direction == CameraMovement::FORWARD) position += front_ * velocity;
        if (direction == CameraMovement::BACKWARD)
            position -= front_ * velocity;
        if (direction == CameraMovement::LEFT) position -= right_ * velocity;
        if (direction == CameraMovement::RIGHT) position += right_ * velocity;

        update_mat4();
    }

    glm::vec3 position;
    glm::vec3 target;
    float yaw;
    float pitch;

    Frustum frustum;

private:
    float calc_pitch(glm::vec3 front) const {
        return glm::degrees(glm::asin(front.y));
    }

    float calc_yaw(glm::vec3 front, float pitch) const {
        return glm::degrees(glm::acos(front.x / cos(glm::radians(pitch))));
    }

    void update_front() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, WORLD_UP));
        up_ = glm::normalize(glm::cross(right_, front_));
    }

    void update_mat4() {
        target = position + front_;
        view_mat_ = glm::lookAt(position, target, up_);
        frustum.update_planes(view_mat_, position);
        projection_mat_ =
            glm::perspective(glm::radians(frustum.fov), frustum.ar,
                             frustum.near_plane, frustum.far_plane);
    }

    glm::vec3 front_;
    glm::vec3 right_;
    glm::vec3 up_;
    float zoom_ = 45;

    glm::mat4 projection_mat_;
    glm::mat4 view_mat_;
};

}  // namespace MR