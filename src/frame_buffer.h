#pragma once

#include <unordered_map>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "shading/texture_cube.h"

namespace MR {

enum class GLAttachmentType {
    MULT_2D_HDR_COL,
    SING_2D_HDR_COL,
    MULT_2D_HDR_DEP,
    SING_2D_HDR_DEP,
    SING_2D_HDR_COL_CLAMP,
    SING_2D_HDR_DEP_BORDER
};

struct FrameBuffer {
    FrameBuffer() {}
    FrameBuffer(int width, int height);

    virtual void init();

    virtual void dispose();

    void bind();
    void unbind();

    void blit_to(const FrameBuffer& fbo, GLbitfield mask);

    void clear(GLbitfield clear_target, glm::vec3 clear_color);

    bool check_completeness(const char* name);

    int width, height;

    GLuint fb_id;
    GLuint attach_color_id;
    GLuint attach_depth_id;
};

struct CaptureFBO : public FrameBuffer {
    CaptureFBO(int w, int h) : FrameBuffer(w, h) {}

    void init() override;

    void dispose() override;

    void resize(int width, int height);

    GLuint rbo_id;
};

struct MultiSampledFBO : public FrameBuffer {
    MultiSampledFBO(int w, int h) : FrameBuffer(w, h) {}

    void init() override;
};

struct ResolveBufferFBO : public FrameBuffer {
    void init() override;

    GLuint blur_high_end;
};

struct QuadHDRBufferFBO : public FrameBuffer {
    void init() override;
};

struct DirShadowBufferFBO : public FrameBuffer {
    DirShadowBufferFBO() : FrameBuffer(2048, 2048) {}
    DirShadowBufferFBO(int w, int h) : FrameBuffer(w, h) {}

    void init() override;
};

struct PointShadowBufferFBO : public FrameBuffer {
    PointShadowBufferFBO() : FrameBuffer(2048, 2048) {}
    PointShadowBufferFBO(int w, int h) : FrameBuffer(w, h) {}

    void init() override;

    // private:
    //     TextureCube tex_drawing_;
};

}  // namespace MR