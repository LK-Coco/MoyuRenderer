#pragma once

#include <memory>
#include "object/object.h"
#include "shading/material.h"
#include "shading/material_property.h"

namespace MR {

struct Entity {
    Entity(std::shared_ptr<Object> obj_) : obj(obj_) {}

    void render(bool use);

    std::shared_ptr<Object> obj;
    // std::shared_ptr<Material> mat;
    MaterialProperty material_prop;
};

}  // namespace MR