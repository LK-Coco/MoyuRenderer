#pragma once

#include <unordered_map>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "shading/texture_cube.h"

namespace MR {

enum class GLAttachmentType {
    COL_MULT_HDR_2D,
    COL_RGBA_HDR_2D,
    COL_RGB_HDR_2D,
    DEP_MULT_HDR_2D,
    DEP_HDR_2D,
    COL_HDR_CLAMP_2D,
    DEP_HDR_BORDER_2D,
    COL_HDR_FLOAT_2D,
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

    GLuint get_attach_color_id(uint8_t index) const {
        return attach_color_ids_[index];
    }

    int width, height;

    GLuint fb_id;
    GLuint attach_depth_id;

protected:
    std::vector<GLuint> attach_color_ids_;
};

struct CaptureFBO : public FrameBuffer {
    CaptureFBO(int w, int h) : FrameBuffer(w, h) {}

    void init() override;

    void dispose() override;

    void resize(int width, int height);

    GLuint rbo_id;
};

struct MultiColorFBO : public FrameBuffer {
    MultiColorFBO(int w, int h) : FrameBuffer(w, h) {}
    void init() override;
};

struct QuadHDRBufferFBO : public FrameBuffer {
    QuadHDRBufferFBO(int w, int h) : FrameBuffer(w, h) {}

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

struct GBufferFBO : public FrameBuffer {
    GBufferFBO() : FrameBuffer(1280, 720) {}
    GBufferFBO(int w, int h) : FrameBuffer(w, h) {}

    void init() override;
};

}  // namespace MR