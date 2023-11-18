#pragma once
#include "glad/glad.h"
#include "object.h"

namespace MR {

class Sphere : public Object {
public:
    Sphere();
    ~Sphere();

    virtual void render() override;

private:
    unsigned int vao_;
    GLsizei index_count_;
};
}  // namespace MR