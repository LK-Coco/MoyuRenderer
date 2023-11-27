#pragma once

#include "glad/glad.h"
#include "shading/texture.h"
#include "render_buffer.h"

namespace MR {

class FrameBuffer {
public:
    FrameBuffer();
    ~FrameBuffer();

    bool is_complete() const;

    void attach_render_buffer(GLenum attachment, RenderBuffer& renderbuffer);

    void attach_texture(GLenum attachment, Texture& texture, GLint level = 0);

    void attach_draw_buffer(GLsizei size, const GLenum* buffers);

    // 显式指定特定区域（如颜色附着等）的数据无效，从而触发相应的清除或重建操作
    void invalidate(GLsizei num_attachments, const GLenum* attachments);

    void invalidate_sub_data(GLsizei num_attachments, const GLenum* attachments,
                             GLint x, GLint y, GLsizei width, GLsizei height);

    void set(GLenum property, GLint value);

    template <class T>
    void clear(GLenum buffer, GLint draw_buffer, const T* value);

    void clear(GLenum buffer, GLint draw_buffer, GLfloat depth, GLint stencil);

    void bind();

    void unbind();

    inline GLuint get_id() const { return fbo_; }
    // inline GLuint get_rbo_id() const { return rbo_; }
    // inline GLuint get_texture_id() const { return texture_id_; }

private:
    GLuint fbo_ = 0;
    // GLuint rbo_ = 0;
    // GLuint texture_id_ = 0;
};

template <>
inline void FrameBuffer::clear<GLint>(GLenum buffer, GLint draw_buffer,
                                      const GLint* value) {
    glClearNamedFramebufferiv(fbo_, buffer, draw_buffer, value);
}

template <>
inline void FrameBuffer::clear<GLuint>(GLenum buffer, GLint draw_buffer,
                                       const GLuint* value) {
    glClearNamedFramebufferuiv(fbo_, buffer, draw_buffer, value);
}

template <>
inline void FrameBuffer::clear<GLfloat>(GLenum buffer, GLint draw_buffer,
                                        const GLfloat* value) {
    glClearNamedFramebufferfv(fbo_, buffer, draw_buffer, value);
}

}  // namespace MR