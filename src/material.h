#pragma once

#include "shader.h"

namespace MR {

class Material {
public:
    Shader* get_shader() const;
    void set_shader(Shader* shader);

private:
    Shader* shader_;
};

}  // namespace MR