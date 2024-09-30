#pragma once

#include "utility.h"
#include "glad/glad.h"

namespace gl {

class RenderBuffer {
public:
    enum class AttachmentType : std::uint32_t {
        depth = GL_DEPTH_ATTACHMENT,
        stencil = GL_STENCIL_ATTACHMENT,
        depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT
    };

    RenderBuffer(Resolution res, AttachmentType type = AttachmentType::depth,
                 TexSamples samples = TexSamples::MSAA_X1) noexcept;

    ~RenderBuffer();

    RenderBuffer(const RenderBuffer&) = delete;
    auto operator=(const RenderBuffer&) -> RenderBuffer& = delete;

    RenderBuffer(RenderBuffer&& other) noexcept;

    auto operator=(RenderBuffer&& other) noexcept -> RenderBuffer&;

    void bind() const;

    void unbind() const;

    [[nodiscard]] constexpr auto id() const noexcept -> std::uint32_t {
        return id_;
    }

    [[nodiscard]] constexpr auto res() const noexcept -> Resolution {
        return res_;
    }

    [[nodiscard]] constexpr auto type() const noexcept -> AttachmentType {
        return type_;
    }

    [[nodiscard]] constexpr auto samples() const noexcept -> TexSamples {
        return samples_;
    }

private:
    std::uint32_t id_{};
    Resolution res_{};
    AttachmentType type_{};
    TexSamples samples_{};
};

inline RenderBuffer::RenderBuffer(Resolution res, AttachmentType type,
                                  TexSamples samples) noexcept
    : res_(res),
      type_(type),
      samples_(samples) {
    std::int32_t internal_format{};

    switch (type_) {
        case AttachmentType::depth:
            internal_format = GL_DEPTH_COMPONENT24;
            break;
        case AttachmentType::stencil:
            internal_format = GL_STENCIL_INDEX8;
            break;
        case AttachmentType::depth_stencil:
            internal_format = GL_DEPTH24_STENCIL8;
            break;
    }

    glGenRenderbuffers(1, &id_);
    glBindRenderbuffer(GL_RENDERBUFFER, id_);
    if (samples_ != TexSamples::MSAA_X1) {
        glRenderbufferStorageMultisample(
            GL_RENDERBUFFER, static_cast<std::int32_t>(samples_),
            internal_format, res.width, res.height);
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, internal_format, res.width,
                              res.height);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

inline RenderBuffer::~RenderBuffer() {
    if (id_ != 0) glDeleteRenderbuffers(1, &id_);
}

inline RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept
    : id_(other.id_),
      res_(other.res_),
      type_(other.type_),
      samples_(other.samples_) {
    other.id_ = 0;
}

inline auto RenderBuffer::operator=(RenderBuffer&& other) noexcept
    -> RenderBuffer& {
    if (this != &other) {
        id_ = other.id_;
        res_ = other.res_;
        type_ = other.type_;
        samples_ = other.samples_;

        other.id_ = 0;
    }

    return *this;
}

inline void RenderBuffer::bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, id_);
}

inline void RenderBuffer::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

}  // namespace gl