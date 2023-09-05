#pragma once

#include <cstdint>
#include "glm/glm.hpp"

namespace MR {

using namespace glm;

// clang-format off
#define UNIFORM_FOREACH(f) \
                f(bool) \
                f(int) \
                f(float) \
                f(vec2) \
                f(vec3) \
                f(vec4) \
                f(mat2) \
                f(mat3) \
                f(mat4)
// clang-format on

enum class UniformType : std::uint8_t {

#define _FUNCTION(name) u_##name,
    UNIFORM_FOREACH(_FUNCTION)
#undef _FUNCTION
};

// clang-format off
#define _FUNCTION(name) struct UniformValue_##name { \
                            UniformType type; \
                            name value; \
                        };
    UNIFORM_FOREACH(_FUNCTION)
#undef _FUNCTION
// clang-format on

}  // namespace MR