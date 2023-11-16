#pragma once

#include "glad/glad.h"

namespace MR {

class FrameBuffer {
public:
    FrameBuffer(int width, int height, bool bind_tex);

    void bind();

    void unbind();

    inline GLuint get_id() const { return fbo_; }

    inline GLuint get_texture_id() const { return texture_id_; }

private:
    GLuint fbo_;
    GLuint texture_id_;
};

}  // namespace MR