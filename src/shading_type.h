#pragma once

#include <cstdint>

namespace MR {

// clang-format off
#define UNIFORM_FOREACH(f) \
                f(BOOL) \
                f(INT) \
                f(FLOAT) \
                f(VEC2) \
                f(VEC3) \
                f(VEC4) \
                f(MAT2) \
                f(MAT3) \
                f(MAT4)
// clang-format on

enum class UniformType : std::uint8_t {

#define _FUNCTION(name) name,
    UNIFORM_FOREACH(_FUNCTION)
#undef _FUNCTION

};
}  // namespace MR