#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

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
        update();
    }

    glm::mat4 get_view_mat() {
        update();
        return glm::lookAt(position, position + front_, up_);
    }

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

        update();
    }

    void on_process_keyboard(CameraMovement direction, float delta_time) {
        float velocity = MovementSpeed * delta_time;
        if (direction == CameraMovement::FORWARD) position += front_ * velocity;
        if (direction == CameraMovement::BACKWARD)
            position -= front_ * velocity;
        if (direction == CameraMovement::LEFT) position -= right_ * velocity;
        if (direction == CameraMovement::RIGHT) position += right_ * velocity;
    }

    glm::vec3 position;
    float yaw;
    float pitch;

private:
    void update() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, WORLD_UP));
        up_ = glm::normalize(glm::cross(right_, front_));
    }

    glm::vec3 front_;
    glm::vec3 right_;
    glm::vec3 up_;
    float zoom_ = 45;
};

}  // namespace MR