#pragma once

#include <map>
#include "shader.h"
#include "shading_type.h"
#include "glm/glm.hpp"

namespace MR {

class Material {
public:
    Shader* get_shader() const;
    void set_shader(Shader* shader);

#define _FUNC(n) void set_##n(std::string name, n value);
    UNIFORM_FOREACH(_FUNC)
#undef _FUNC

private:
    Shader* shader_;

#define _FUNC(name) std::map<std::string, UniformValue_##name> map_uniform_##name##_;
    UNIFORM_FOREACH(_FUNC)
#undef _FUNC
};

}  // namespace MR