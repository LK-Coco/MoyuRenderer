#pragma once

#include "object.h"
#include "glad/glad.h"

namespace MR {

class Quad : public Object {
public:
    Quad();
    ~Quad();

    virtual void render() override;

private:
    GLuint vao_;
};

}  // namespace MR