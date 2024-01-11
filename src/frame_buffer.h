#pragma once

#include <unordered_map>
#include "glm/glm.hpp"
#include "glad/glad.h"

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
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    virtual void init();
    virtual void deinit();

    GLuint attach(GLAttachmentType attach_type, unsigned int attach_index);

    void bind();
    void unbind();

    void blit_to(const FrameBuffer& fbo, GLbitfield mask);

    void clear(GLbitfield clear_target, glm::vec3 clear_color);

    bool check_completeness();

    int width, height;

    // std::unordered_map<GLAttachmentType, std::vector<GLuint>> attaches;

    GLuint fb_id;
    GLuint attach_color_id;
    GLuint attach_depth_id;
};

struct CaptureFBO : public FrameBuffer {
    void init() override;
    void deinit() override;

    void resize(int width, int height);

    GLuint rbo_id;
};

}  // namespace MR