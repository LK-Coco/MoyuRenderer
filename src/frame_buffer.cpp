#include <iostream>
#include "frame_buffer.h"
#include "scene.h"

namespace MR {

GLuint attach(GLAttachmentType attach_type, unsigned int attach_index,
              std::uint32_t width, std::uint32_t height) {
    const float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    switch (attach_type) {
        case GLAttachmentType::COL_RGB_HDR_2D:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                         GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case GLAttachmentType::COL_MULT_HDR_2D:
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F,
                                    width, height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attach_index,
                                   GL_TEXTURE_2D_MULTISAMPLE, tex_id, 0);
            break;
        case GLAttachmentType::COL_RGBA_HDR_2D:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                         GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attach_index,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
        case GLAttachmentType::DEP_MULT_HDR_2D:
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4,
                                    GL_DEPTH_COMPONENT32F, width, height,
                                    GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D_MULTISAMPLE, tex_id, 0);
            break;
        case GLAttachmentType::DEP_HDR_2D:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height,
                         0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
        case GLAttachmentType::COL_HDR_CLAMP_2D:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attach_index,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
        case GLAttachmentType::DEP_HDR_BORDER_2D:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height,
                         0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

        case GLAttachmentType::COL_HDR_FLOAT_2D:
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                         GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attach_index,
                                   GL_TEXTURE_2D, tex_id, 0);
            break;
    }

    return tex_id;
}

FrameBuffer::FrameBuffer(int w, int h) {
    width = w;
    height = h;
}

void FrameBuffer::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    auto attach_color_id =
        attach(GLAttachmentType::COL_RGB_HDR_2D, 0, width, height);
    attach_color_ids_.push_back(attach_color_id);
    attach_depth_id = attach(GLAttachmentType::DEP_HDR_2D, 0, width, height);

    check_completeness("FrameBuffer");
}

void FrameBuffer::dispose() { glDeleteFramebuffers(1, &fb_id); }

void FrameBuffer::clear(GLbitfield clear_target, glm::vec3 clear_color) {
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
    glClear(clear_target);
}

void FrameBuffer::bind() {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);
}

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

bool FrameBuffer::check_completeness(const char* name) {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        GLuint code = glGetError();
        std::cout << "ERROR::" << name << "  " << code
                  << ":: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return false;
    }
    std::cout << name << "::init ok" << std::endl;
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

    check_completeness("CaptureFBO");
}

void CaptureFBO::dispose() {
    glDeleteFramebuffers(1, &fb_id);
    glDeleteRenderbuffers(1, &rbo_id);
}

void CaptureFBO::resize(int w, int h) {
    width = w;
    height = h;
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
}

void MultiColorFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    auto attach_color_id =
        attach(GLAttachmentType::COL_HDR_FLOAT_2D, 0, width, height);
    auto attach_color_1_id =
        attach(GLAttachmentType::COL_HDR_FLOAT_2D, 1, width, height);

    attach_color_ids_.push_back(attach_color_id);
    attach_color_ids_.push_back(attach_color_1_id);

    attach_depth_id = attach(GLAttachmentType::DEP_HDR_2D, 0, width, height);

    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    check_completeness("MultiColorFBO");
}

void QuadHDRBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    auto attach_color_id =
        attach(GLAttachmentType::COL_HDR_FLOAT_2D, 0, width, height);
    attach_color_ids_.push_back(attach_color_id);

    check_completeness("QuadHDRBufferFBO");
}

void DirShadowBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    attach_depth_id =
        attach(GLAttachmentType::DEP_HDR_BORDER_2D, 0, width, height);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    check_completeness("DirShadowBufferFBO");
}

void PointShadowBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);

    GLuint depth_cubemap;
    glGenTextures(1, &depth_cubemap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);
    for (GLuint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    attach_depth_id = depth_cubemap;

    check_completeness("PointShadowBufferFBO");
}

void GBufferFBO::init() {
    glGenFramebuffers(1, &fb_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    gAlbedo = attach(GLAttachmentType::COL_RGBA_HDR_2D, 0, width, height);
    gNormal = attach(GLAttachmentType::COL_RGBA_HDR_2D, 1, width, height);
    gPosition = attach(GLAttachmentType::COL_RGBA_HDR_2D, 2, width, height);

    // attach_color_ids_.push_back(gAlbedo);
    // attach_color_ids_.push_back(gNormal);
    // attach_color_ids_.push_back(gPosition);

    attach_depth_id = attach(GLAttachmentType::DEP_HDR_2D, 0, width, height);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                   GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    check_completeness("GBufferFBO");
}

}  // namespace MR