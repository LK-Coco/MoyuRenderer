#pragma once

#include "base.h"
#include "entity.h"
#include "material.h"

namespace MR {

class Model : public Entity {
public:
    bool lighting = true;
    Ref<Material> mat;
};

}  // namespace MR