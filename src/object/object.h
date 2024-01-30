#pragma once

#include "glm/glm.hpp"

namespace MR {

class Object {
public:
    virtual ~Object(){};
    virtual void render() = 0;

    glm::vec3 position{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    glm::vec3 eulerAngles{0, 0, 0};
};

}  // namespace MR