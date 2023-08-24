#pragma once
#include <memory>
#include "shader.h"

namespace MR {

class Object {
public:
    virtual ~Object(){};
    virtual void render(std::shared_ptr<Shader>& shader) = 0;
};
}  // namespace MR