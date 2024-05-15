#pragma once
#include "renderer.h"
#include "skybox.h"
#include "frame_buffer.h"
#include "shading/material_property.h"
#include "object/screen_quad.h"
#include "object/sphere.h"
#include "light.h"

namespace MR {

enum class RenderMode {
    Forward,
    Deferred,
};

#define DEFERRED_LIGHT_COUNT 32

class Rasterizer : public Renderer {
public:
    Rasterizer();

    void render() override;

    GLuint get_image_id() const override;

    RenderMode render_mode = RenderMode::Deferred;

private:
    void load_shaders();

    void init_ssbo();

    void pre_process();

    void forward_render();

    void deferred_render();

    void draw_point_light_shadow_forward();

    void draw_dir_light_shadow_forward();

    void post_process_forward();

    void draw_depth_pass();

    void light_assignment();

    void draw_objects();

    void post_process();

    MultiColorFBO multi_color_fbo_;
    DirShadowBufferFBO dir_shadow_fbo_;
    std::vector<PointShadowBufferFBO> point_shadow_fbos_;
    std::vector<QuadHDRBufferFBO> ping_pong_fbos_;
    FrameBuffer fbo_;

    // defferred
    GBufferFBO gbuffer_fbo_;
    Shader gbuffer_shader_;
    Shader deferred_shading_shader_;
    Shader show_light_shader_;
    Sphere light_sphere_;

    // ssao
    std::vector<glm::vec3> ssao_kernel_;
    std::vector<float> ssao_noise_;
    Shader ssao_shader_;
    GLuint ssao_fbo_;
    GLuint ssao_fbo_color_;
    GLuint ssao_noise_tex_;
    QuadHDRBufferFBO ssao_blur_fbo_;
    Shader ssao_blur_shader_;

    GPULight lights_deferred_[DEFERRED_LIGHT_COUNT];

    Shader skybox_shader_;
    Shader dir_light_shader_;
    Shader point_light_shader_;
    Shader depth_shader_;
    Shader cluster_cull_light_shader_;
    Shader pbr_shader_;
    Shader pbr_cluster_shader_;
    Shader build_aabb_grid_comp_shader_;
    Shader blur_shader_;
    Shader bloom_shader_;
    Shader light_mesh_shader_;

    ScreenQuad screen_quad_;
    Cube light_cube_;

    const unsigned int GRID_SIZE_X = 16;
    const unsigned int GRID_SIZE_Y = 9;
    const unsigned int GRID_SIZE_Z = 24;
    const unsigned int NUM_CLUSTERS = GRID_SIZE_X * GRID_SIZE_Y * GRID_SIZE_Z;
    unsigned int size_x_, size_y_;

    unsigned int num_lights_;
    const unsigned int MAX_LIGHTS = 1000;
    const unsigned int MAX_LIGHTS_PER_TILE = 50;

    unsigned int aabb_volume_grid_ssbo_, screen_to_view_ssbo_;
    unsigned int light_ssbo_, light_index_list_ssbo_, light_grid_ssbo_,
        light_index_global_count_ssbo_;
};

}  // namespace MR