#pragma once

#include "glm/glm.hpp"
#include "hit_result.h"

namespace MR {

struct Physics {
    static bool ray_cast(const glm::vec3& origin, const glm::vec3& direction,
                         HitResult& result);
};

}  // namespace MR