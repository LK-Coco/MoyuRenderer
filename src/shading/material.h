#pragma once

#include <unordered_map>
#include "shader.h"
#include "shading_type.h"
#include "glm/glm.hpp"

namespace MR {

class Material {
public:
    virtual void display_ui(){};

    virtual void fill_unifrom() = 0;

    Shader* get_shader() const;
    void set_shader(Shader* shader);

#define _FUNC(n) void set_##n(std::string name, n value);
    UNIFORM_FOREACH(_FUNC)
#undef _FUNC

protected:
    Shader* shader_;

#define _FUNC(name) \
    std::unordered_map<std::string, UniformValue_##name> map_uniform_##name##_;
    UNIFORM_FOREACH(_FUNC)
#undef _FUNC
    std::unordered_map<std::string, UniformValue_Sampler> map_unifrom_sampler;
    std::unordered_map<std::string, std::string> map_sampler_to_tex_name;
};

}  // namespace MR