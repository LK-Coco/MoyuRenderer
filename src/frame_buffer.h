#pragma once

#include "glad/glad.h"

namespace MR {

class FrameBuffer {
public:
    FrameBuffer(int width, int height, bool bind_tex);
    ~FrameBuffer();

    void resize(int width, int height);

    void bind();

    void unbind();

    inline GLuint get_id() const { return fbo_; }
    inline GLuint get_rbo_id() const { return rbo_; }
    inline GLuint get_texture_id() const { return texture_id_; }

private:
    GLuint fbo_;
    GLuint rbo_;
    GLuint texture_id_ = 0;
};

}  // namespace MR