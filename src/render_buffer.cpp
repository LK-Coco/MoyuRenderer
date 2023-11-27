#include "render_buffer.h"

namespace MR {

RenderBuffer::RenderBuffer() { glGenRenderbuffers(1, &rbo_); }

RenderBuffer::~RenderBuffer() { release(); }

GLuint RenderBuffer::get_id() const { return rbo_; }

void RenderBuffer::release() {
    if (rbo_) glDeleteRenderbuffers(1, &rbo_);
}

void RenderBuffer::set_storage(GLenum internal_format, GLsizei width,
                               GLsizei height) {
    glNamedRenderbufferStorage(rbo_, internal_format, width, height);
}

void RenderBuffer::set_storage_multisample(GLsizei samples,
                                           GLenum internal_format,
                                           GLsizei width, GLsizei height) {
    glNamedRenderbufferStorageMultisample(rbo_, samples, internal_format, width,
                                          height);
}

void RenderBuffer::get_parameteriv(GLenum property, GLint* value) {
    glGetNamedRenderbufferParameteriv(rbo_, property, value);
}

}  // namespace MR