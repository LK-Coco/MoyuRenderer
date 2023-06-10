#pragma once

#include <string>
#include "glm_inc.h"

namespace MR {

class Entity {
protected:
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

}  // namespace MR