#pragma once

#include "base.h"
#include "entity.h"
#include "material.h"
#include "triangle.h"

namespace MR {

class Model : public Entity {
public:
    Model(const std::string& file_path);

    bool lighting = true;
    Ref<Material> mat;
    std::vector<Triangle*> triangle_list;
};

}  // namespace MR