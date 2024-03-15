#pragma once

#include "transform.h"
#include "ray.h"

namespace MR {

class Object : public Transform {
public:
    virtual ~Object(){};
    virtual void render() = 0;

    virtual bool hit(Ray& ray) { return false; }
};

}  // namespace MR