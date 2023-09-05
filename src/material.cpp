#include "material.h"

namespace MR {

#define _FUNC(n)                                            \
    void Material::set_##n(std::string name, n value) {     \
        map_uniform_##n##_[name].type = UniformType::u_##n; \
        map_uniform_##n##_[name].value = value;             \
    }
UNIFORM_FOREACH(_FUNC)
#undef _FUNC

}  // namespace MR