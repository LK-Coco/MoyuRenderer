#pragma once
#include "renderer.h"
#include "skybox.h"
#include "frame_buffer.h"

namespace MR {

class Rasterizer : public Renderer {
public:
    Rasterizer();

    void render() override;

    void render_skybox(std::shared_ptr<Skybox>& skybox) override;

    GLuint get_image_id() const override;

private:
    void load_shaders();

    void draw_point_light_shadow();

    void draw_dir_light_shadow();

    void draw_depth_pass();

    void draw_objects();

    void post_process();

    FrameBuffer fbo_;
    DirShadowBufferFBO dir_shadow_fbo_;

    Shader dir_light_shader_;
    Shader depth_shader_;
};

}  // namespace MR