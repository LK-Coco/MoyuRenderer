#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace MR {

constexpr glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);

class Camera {
public:
    Camera(glm::vec3 pos, glm::vec3 center, float yaw = 90.0f, float pitch = 0.0f)
        : position(pos),
          center(center),
          yaw(yaw),
          pitch(pitch) {
        zoom_ = glm::length(position - center);
        update();
    }

    glm::mat4 get_view_mat() const { return glm::lookAt(position, center, up_); }

    float get_zoom() const { return zoom_; }

    void set_zoom(float zoom) {
        zoom_ = zoom;
        position = center - front_ * zoom;
    }

    void on_process_mouse_scroll(float scroll) {
        zoom_ -= scroll;
        zoom_ = glm::clamp(zoom_, 1.0f, 100.0f);
        set_zoom(zoom_);
    }

    void on_process_mouse_move(float x_offset, float y_offset) {
        yaw -= x_offset;
        pitch -= y_offset;
        pitch = glm::clamp(pitch, -88.0f, 89.0f);

        position.x = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch)) * zoom_;
        position.y = glm::sin(glm::radians(pitch)) * zoom_;
        position.z = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)) * zoom_;
        position += center;

        update();
    }

    glm::vec3 position;
    glm::vec3 center;
    float yaw;
    float pitch;

private:
    void update() {
        front_ = glm::normalize(center - position);
        right_ = glm::normalize(glm::cross(front_, WORLD_UP));
        up_ = glm::normalize(glm::cross(right_, front_));
    }

    glm::vec3 front_;
    glm::vec3 right_;
    glm::vec3 up_;
    float zoom_;
};

}  // namespace MR