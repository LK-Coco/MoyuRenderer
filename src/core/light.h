#pragma once

#include "entity.h"
#include "color.h"

namespace MR {

class Light : public Entity {
public:
    Color color;
};

}  // namespace MR