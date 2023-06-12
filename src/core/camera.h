#pragma once

#include "entity.h"

namespace MR {

class Camera : public Entity {
protected:
    glm::vec3 up_;
    glm::vec3 forward_;
};

}  // namespace MR
