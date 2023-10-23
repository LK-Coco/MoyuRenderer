#pragma once
#include <memory>
#include "shading/material.h"

namespace MR {

class Object {
public:
    virtual ~Object(){};
    virtual void render(std::shared_ptr<Material>& mat) = 0;
};
}  // namespace MR