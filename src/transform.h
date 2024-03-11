#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

namespace MR {

class Transform {
public:
    Transform()
        : translation(0.0f, 0.0f, 0.0f),
          rotation(0.0f, 0.0f, 0.0f),
          scale(1.0f, 1.0f, 1.0f) {}
    Transform(const glm::vec3& pos)
        : translation(pos),
          rotation(0.0f, 0.0f, 0.0f),
          scale(1.0f, 1.0f, 1.0f) {}

    glm::mat4 get_transform_mat4() const {
        // glm::mat4 rotate =
        //     glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z),
        //                 glm::vec3(0, 0, 1)) *
        //     glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x),
        //                 glm::vec3(1, 0, 0)) *
        //     glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y),
        //                 glm::vec3(0, 1, 0));

        glm::mat4 rotate = glm::toMat4(glm::quat(
            glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y),
                      glm::radians(rotation.z))));
        return glm::translate(glm::mat4(1.0f), translation) * rotate *
               glm::scale(glm::mat4(1.0f), scale);
    }

public:
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
};

}  // namespace MR