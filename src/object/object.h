#pragma once

#include "transform.h"

namespace MR {

class Object : public Transform {
public:
    virtual ~Object(){};
    virtual void render() = 0;
};

}  // namespace MR