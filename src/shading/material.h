#pragma once

#include <memory>
#include <unordered_map>
#include "shader.h"
#include "shading_type.h"
#include "glm/glm.hpp"
#include "object/object.h"

namespace MR {

class Material {
public:
    virtual void init() = 0;

    virtual void set_uniform(std::shared_ptr<Object> obj) = 0;

    const Shader& get_shader() const { return shader_; }

protected:
    Shader shader_;
};

}  // namespace MR