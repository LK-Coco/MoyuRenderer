#pragma once
#include <memory>
#include "shading/material.h"

namespace MR {

class Object {
public:
    virtual ~Object(){};
    virtual void render() = 0;
};
}  // namespace MR