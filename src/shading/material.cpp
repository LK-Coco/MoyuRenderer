#include "material.h"

namespace MR {

Shader* Material::get_shader() const { return shader_; }
void Material::set_shader(Shader* shader) { shader_ = shader; }

#define _FUNC(n)                                            \
    void Material::set_##n(std::string name, n value) {     \
        map_uniform_##n##_[name].type = UniformType::u_##n; \
        map_uniform_##n##_[name].value = value;             \
    }
UNIFORM_FOREACH(_FUNC)
#undef _FUNC

}  // namespace MR