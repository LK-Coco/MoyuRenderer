#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"

namespace MR {

class Object {
public:
    virtual ~Object(){};
    virtual void render() = 0;

    glm::vec3 position{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    glm::vec3 rotation_axis{1, 0, 0};
    float angle{0};

    inline glm::mat4 get_model_matrix() const {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, angle, rotation_axis);
        matrix = glm::scale(matrix, scale);

        return matrix;
    }
};

}  // namespace MR