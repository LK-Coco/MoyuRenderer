#pragma once

#include "shading/shader.h"
#include "glad/glad.h"
#include "frame_buffer.h"

namespace MR {

class Pass {
public:
    virtual void init() {}

    virtual void render() {}

    virtual void deinit() {}

    virtual GLuint get_tex_id() { return 0; }
};

}  // namespace MR