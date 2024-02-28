#pragma once

#include "object.h"
#include "glad/glad.h"

namespace MR {

class ScreenQuad : public Object {
public:
    ScreenQuad();
    ~ScreenQuad();

    void render() override;

private:
    GLuint vao_;
};

}  // namespace MR