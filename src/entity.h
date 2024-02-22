#pragma once

#include <memory>
#include "object/object.h"
#include "shading/material.h"

namespace MR {

struct Entity {
    void render(bool use);

    std::shared_ptr<Object> obj;
    std::shared_ptr<Material> mat;
};

}  // namespace MR