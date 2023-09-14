#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace MR {

constexpr glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);

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

    void set_zoom(float zoom) {
        zoom_ = zoom;
        // position = center - front_ * zoom;
    }

    void on_process_mouse_scroll(float scroll) {
        zoom_ -= (float)scroll;
        if (zoom_ < 1.0f) zoom_ = 1.0f;
        if (zoom_ > 99.0f) zoom_ = 99.0f;
    }

    void on_process_mouse_move(float x_offset, float y_offset) {
        // TODO 重构：四元数的旋转
        yaw += x_offset * 0.5f;
        pitch += y_offset * 0.5f;
        pitch = glm::clamp(pitch, -88.0f, 89.0f);
        if (yaw > 360.f) yaw -= 360.f;
        if (yaw < -360.f) yaw += 360.f;

        // float n = glm::cos(glm::radians(pitch)) * zoom_;

        // position.x = glm::cos(glm::radians(yaw)) * n;
        // position.y = glm::sin(glm::radians(pitch)) * zoom_;
        // position.z = glm::sin(glm::radians(yaw)) * n;

        update();
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