#pragma once

#include <optional>
#include <vector>
#include "glm/glm.hpp"
#include "texture.h"
#include "render_buffer.h"

namespace gl {

enum class RenderBufferType : std::uint8_t {
    NONE = 0x00,
    ATTACH_DEPTH_BUFFER = 0x01,
    ATTACH_STENCIL_BUFFER = 0x02,
    ATTACH_DEPTH_STENCIL_BUFFER = 0x03,
};

class FrameBuffer {
public:
    FrameBuffer() noexcept;
    ~FrameBuffer();

    FrameBuffer(const FrameBuffer&) = delete;
    auto operator=(const FrameBuffer&) -> FrameBuffer& = delete;

    FrameBuffer(FrameBuffer&& other) noexcept;
    auto operator=(FrameBuffer&& other) noexcept -> FrameBuffer&;

    template <GLenum type>
    void attach_color(const Texture<type>& tex, size_t index,
                      const GLint level = 0) {
        glNamedFramebufferTexture(id_, GL_COLOR_ATTACHMENT0 + index, tex.id(),
                                  level);
    }

    template <GLenum type>
    void attach_depth(const Texture<type>& tex) {
        glNamedFramebufferTexture(id_, GL_DEPTH_ATTACHMENT, tex.id(), 0);
    }

    void set_renderbuffer(
        Resolution res,
        RenderBufferType attachment = RenderBufferType::ATTACH_DEPTH_BUFFER,
        TexSamples samples = TexSamples::MSAA_X1);

    void bind() const;
    void unbind() const;

    void blit(const FrameBuffer& source, const GLint source_x,
              const GLint source_y, const GLint source_w, const GLint source_h,
              const GLint x, const GLint y, const GLint w, const GLint h,
              const GLbitfield mask, const GLenum filter) const {
        glBlitNamedFramebuffer(source.id_, id_, source_x, source_y, source_w,
                               source_h, x, y, w, h, mask, filter);
    }

    void set_draw_buffer(const GLenum buffer) const {
        glNamedFramebufferDrawBuffer(id_, buffer);
    }
    void set_draw_buffers(const std::vector<GLenum>& buffers) const {
        glNamedFramebufferDrawBuffers(id_, static_cast<GLsizei>(buffers.size()),
                                      buffers.data());
    }

    void set_read_buffer(const GLenum mode) const {
        glNamedFramebufferReadBuffer(id_, mode);
    }

    [[nodiscard]] GLuint id() const { return id_; }

    static void set_viewport(Resolution res) {
        glViewport(0, 0, res.width, res.height);
    }

    static void bind_default() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    static void clear(GLbitfield clear_target, const glm::vec4& clear_color);

private:
    GLuint id_{};
    RenderBufferType attachment_;
    std::optional<RenderBuffer> render_buffer_;
};

inline FrameBuffer::FrameBuffer() noexcept { glCreateFramebuffers(1, &id_); }
inline FrameBuffer::~FrameBuffer() {
    if (id_ != 0) glDeleteFramebuffers(1, &id_);
}

inline FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : id_(other.id_),
      attachment_(other.attachment_),
      render_buffer_(std::move(other.render_buffer_)) {
    other.id_ = 0;
}

inline auto FrameBuffer::operator=(FrameBuffer&& other) noexcept
    -> FrameBuffer& {
    if (this != &other) {
        id_ = other.id_;
        attachment_ = other.attachment_;
        render_buffer_ = std::move(other.render_buffer_);
        other.id_ = 0;
    }
    return *this;
}

inline void FrameBuffer::set_renderbuffer(Resolution res,
                                          RenderBufferType attachment,
                                          TexSamples samples) {
    if (attachment != RenderBufferType::NONE) {
        RenderBuffer::AttachmentType type{};

        switch (attachment) {
            case RenderBufferType::ATTACH_DEPTH_BUFFER:
                type = RenderBuffer::AttachmentType::depth;
                break;
            case RenderBufferType::ATTACH_STENCIL_BUFFER:
                type = RenderBuffer::AttachmentType::stencil;
                break;
            case RenderBufferType::ATTACH_DEPTH_STENCIL_BUFFER:
                type = RenderBuffer::AttachmentType::depth_stencil;
                break;
            default: std::terminate();
        }

        attachment_ = attachment;

        render_buffer_.emplace(res, type, samples);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  static_cast<std::uint32_t>(type),
                                  GL_RENDERBUFFER, render_buffer_->id());
    } else {
        if (attachment_ == RenderBufferType::NONE) return;

        RenderBuffer::AttachmentType type{};

        switch (attachment_) {
            case RenderBufferType::ATTACH_DEPTH_BUFFER:
                type = RenderBuffer::AttachmentType::depth;
                break;
            case RenderBufferType::ATTACH_STENCIL_BUFFER:
                type = RenderBuffer::AttachmentType::stencil;
                break;
            case RenderBufferType::ATTACH_DEPTH_STENCIL_BUFFER:
                type = RenderBuffer::AttachmentType::depth_stencil;
                break;
            default: break;
        }

        attachment_ = RenderBufferType::NONE;

        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  static_cast<std::uint32_t>(type),
                                  GL_RENDERBUFFER, 0);
    }
}

inline void FrameBuffer::clear(GLbitfield clear_target,
                               const glm::vec4& clear_color) {
    glClearColor(clear_color[0], clear_color[1], clear_color[2],
                 clear_color[3]);
    glClear(clear_target);
}

inline void FrameBuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

inline void FrameBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace gl