#pragma once

#include "glad/glad.h"

namespace MR {

class FrameBuffer {
public:
    FrameBuffer();

    void bind();

    void unbind();

    inline GLuint get_id() const { return fbo_; }

private:
    GLuint fbo_;
    GLuint texture_id_;
};

}  // namespace MR