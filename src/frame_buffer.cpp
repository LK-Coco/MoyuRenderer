#include <iostream>
#include "frame_buffer.h"
#include "scene.h"

namespace MR {

GLuint attach(GLAttachmentType attach_type, unsigned int attach_index,
              std::uint32_t width, std::uint32_t height) {
    const float border_color[] = {0.0f, 0.0f, 0.0f, 1.0f};

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    switch (attach_type) {
        case GLAttachmentType::MULT_2D_HDR_COL:
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F,
                                    width, height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attach_index,
                                   GL_TEXTURE_2D_MULTISAMPLE, tex_id, 0);
            break;
        case GLAttachmentType::SING_2D_HDR_COL:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attach_index,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
        case GLAttachmentType::MULT_2D_HDR_DEP:
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4,
                                    GL_DEPTH_COMPONENT32F, width, height,
                                    GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D_MULTISAMPLE, tex_id, 0);
            break;
        case GLAttachmentType::SING_2D_HDR_DEP:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height,
                         0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
        case GLAttachmentType::SING_2D_HDR_COL_CLAMP:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attach_index,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
        case GLAttachmentType::SING_2D_HDR_DEP_BORDER:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height,
                         0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                            GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                            GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                            GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
                             border_color);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
    }

    return tex_id;
}

FrameBuffer::FrameBuffer(int w, int h) {
    width = w;
    height = h;

    init();
}

FrameBuffer::~FrameBuffer() { deinit(); }

void FrameBuffer::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    attach_color_id =
        attach(GLAttachmentType::SING_2D_HDR_COL, 0, width, height);
    attach_depth_id =
        attach(GLAttachmentType::SING_2D_HDR_DEP, 0, width, height);

    check_completeness();
}

void FrameBuffer::deinit() { glDeleteFramebuffers(1, &fb_id); }

void FrameBuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, fb_id); }

void FrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void FrameBuffer::blit_to(const FrameBuffer& fbo, GLbitfield mask) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.fb_id);
    if ((mask & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT) {
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask,
                      GL_NEAREST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_id);
}

bool FrameBuffer::check_completeness() {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                  << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void CaptureFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    glGenRenderbuffers(1, &rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    check_completeness();
}

void CaptureFBO::deinit() {
    glDeleteFramebuffers(1, &fb_id);
    glDeleteRenderbuffers(1, &rbo_id);
}

void CaptureFBO::resize(int w, int h) {
    width = w;
    height = h;
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
}

void MultiSampledFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    attach_color_id =
        attach(GLAttachmentType::MULT_2D_HDR_COL, 0, width, height);
    attach_depth_id =
        attach(GLAttachmentType::MULT_2D_HDR_DEP, 0, width, height);

    check_completeness();
}

void ResolveBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    attach_color_id =
        attach(GLAttachmentType::SING_2D_HDR_COL, 0, width, height);
    attach_depth_id =
        attach(GLAttachmentType::SING_2D_HDR_DEP, 0, width, height);
    blur_high_end =
        attach(GLAttachmentType::SING_2D_HDR_COL_CLAMP, 1, width, height);

    check_completeness();
}

void ResolveBufferFBO::deinit() { glDeleteFramebuffers(1, &fb_id); }

void QuadHDRBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    attach_color_id =
        attach(GLAttachmentType::SING_2D_HDR_COL_CLAMP, 0, width, height);

    check_completeness();
}

void QuadHDRBufferFBO::deinit() { glDeleteFramebuffers(1, &fb_id); }

void DirShadowBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    attach_depth_id =
        attach(GLAttachmentType::SING_2D_HDR_DEP_BORDER, 0, width, height);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    check_completeness();
}

void DirShadowBufferFBO::deinit() { glDeleteFramebuffers(1, &fb_id); }

void PointShadowBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    tex_drawing.generate_texture_id();
    tex_drawing.bind();
    tex_drawing.generate_with_type(width, height, CubeMapType::SHADOW_MAP);
    attach_depth_id = tex_drawing.id;

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attach_depth_id,
                         0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    check_completeness();
}

void PointShadowBufferFBO::deinit() { glDeleteFramebuffers(1, &fb_id); }

}  // namespace MR