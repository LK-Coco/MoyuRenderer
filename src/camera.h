#pragma once

#include "transform.h"
#include "geometry.h"
#include "jyu_app/src/input.h"

namespace MR {

constexpr glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);
constexpr float MovementSpeed = 5.0f;

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera : public Transform {
public:
    Camera(glm::vec3 pos, float yaw = -90.0f, float pitch = 0.0f)
        : Transform(pos),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          yaw(yaw),
          pitch(pitch) {
        update_front();
    }

    Camera(glm::vec3 pos, glm::vec3 tar, float fov, float ar, float near_p,
           float far_p)
        : Transform(pos) {
        target = tar;
        front_ = glm::normalize(target - translation);
        right_ = glm::normalize(glm::cross(front_, WORLD_UP));
        up_ = glm::normalize(glm::cross(right_, front_));
        pitch = calc_pitch(front_);
        yaw = calc_yaw(front_, pitch);

        rotation.x = -pitch;
        rotation.y = -yaw;

        frustum.fov = fov;
        frustum.ar = ar;
        frustum.near_plane = near_p;
        frustum.far_plane = far_p;

        projection_mat_ =
            glm::perspective(glm::radians(frustum.fov), frustum.ar,
                             frustum.near_plane, frustum.far_plane);
        view_mat_ = glm::lookAt(translation, target, up_);
        frustum.update_planes(view_mat_, translation);
    }

    bool check_visibility(const AABB& bounds) {
        return frustum.check_if_inside(bounds);
    }

    glm::mat4 get_projection() const { return projection_mat_; }

    glm::mat4 get_view_mat() const { return view_mat_; }

    float get_zoom() const { return zoom_; }

    void update(float dt) {
        auto mouse_pos = Jyu::Input::get_mouse_position();
        // TODO need refactor
        float cur_x = 1280 - static_cast<float>(mouse_pos.x);
        float cur_y = 720 - static_cast<float>(mouse_pos.y);
        if (Jyu::Input::is_mouse_button_down(Jyu::MouseButton::Right)) {
            float delta_x = cur_x - mouse_pos_x_;
            float delta_y = mouse_pos_y_ - cur_y;
            on_process_mouse_move(delta_x, delta_y);
        }
        mouse_pos_x_ = cur_x;
        mouse_pos_y_ = cur_y;

        if (Jyu::Input::is_key_down(Jyu::KeyCode::W)) {
            on_process_keyboard(CameraMovement::FORWARD, dt);
        }
        if (Jyu::Input::is_key_down(Jyu::KeyCode::A)) {
            on_process_keyboard(CameraMovement::LEFT, dt);
        }
        if (Jyu::Input::is_key_down(Jyu::KeyCode::D)) {
            on_process_keyboard(CameraMovement::RIGHT, dt);
        }
        if (Jyu::Input::is_key_down(Jyu::KeyCode::S)) {
            on_process_keyboard(CameraMovement::BACKWARD, dt);
        }
    }

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
        if (direction == CameraMovement::FORWARD)
            translation += front_ * velocity;
        if (direction == CameraMovement::BACKWARD)
            translation -= front_ * velocity;
        if (direction == CameraMovement::LEFT) translation -= right_ * velocity;
        if (direction == CameraMovement::RIGHT)
            translation += right_ * velocity;

        update_mat4();
    }

    glm::vec3 target;
    float yaw;
    float pitch;

    Frustum frustum;

private:
    float calc_pitch(glm::vec3 front) const {
        return glm::degrees(glm::asin(front.y));
    }

    float calc_yaw(glm::vec3 front, float pitch) const {
        return glm::degrees(glm::atan(
            front.x / front.z));  //(front.x / cos(glm::radians(pitch))));
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
        target = translation + front_;
        view_mat_ = glm::lookAt(translation, target, up_);
        frustum.update_planes(view_mat_, translation);
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

    float mouse_pos_x_ = 0;
    float mouse_pos_y_ = 0;
};

}  // namespace MR