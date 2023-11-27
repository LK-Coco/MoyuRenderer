#pragma once

#include "glad/glad.h"

namespace MR {

class RenderBuffer {
public:
    RenderBuffer();
    ~RenderBuffer();

    RenderBuffer(const RenderBuffer& other) = delete;
    RenderBuffer& operator=(const RenderBuffer& other) = delete;

    RenderBuffer(RenderBuffer&& other) = default;

public:
    GLuint get_id() const;

    void release();

    void set_storage(GLenum internal_format, GLsizei width, GLsizei height);

    void set_storage_multisample(GLsizei samples, GLenum internal_format,
                                 GLsizei width, GLsizei height);

    void get_parameteriv(GLenum property, GLint* value);

private:
    GLuint rbo_;
};

}  // namespace MR