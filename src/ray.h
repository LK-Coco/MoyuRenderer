#pragma once

#include "glm/glm.hpp"

namespace MR {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    float distance;
};

}  // namespace MR