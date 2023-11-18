#pragma once

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

namespace MR {

struct PointLight {
    glm::vec3 color;
    glm::vec3 position;

    PointLight(const glm::vec3& c, const glm::vec3& pos)
        : color(c),
          position(pos) {}
    virtual ~PointLight() = default;
};

struct DirLight : public PointLight {
    glm::vec3 direction;

    explicit DirLight(const glm::vec3& pos, const glm::vec3& dir,
                      const glm::vec3& c)
        : direction(dir),
          PointLight(c, pos) {}
};

}  // namespace MR