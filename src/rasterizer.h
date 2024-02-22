#pragma once
#include "renderer.h"
#include "skybox.h"
#include "frame_buffer.h"
#include "shading/material_property.h"

namespace MR {

class Rasterizer : public Renderer {
public:
    Rasterizer();

    void render() override;

    void render_skybox(std::shared_ptr<Skybox>& skybox) override;

    GLuint get_image_id() const override;

private:
    void load_shaders();

    void init_ssbo();

    void pre_process();

    void draw_point_light_shadow();

    void draw_dir_light_shadow();

    void draw_depth_pass();

    void light_assignment();

    void draw_objects();

    void post_process();

    FrameBuffer fbo_;
    DirShadowBufferFBO dir_shadow_fbo_;
    MultiSampledFBO multi_sample_fbo_;
    PointShadowBufferFBO* point_shadow_fbos_;

    Shader dir_light_shader_;
    Shader point_light_shader_;
    Shader depth_shader_;
    Shader cluster_cull_light_shader_;
    Shader pbr_cluster_shader_;
    Shader build_aabb_grid_comp_shader_;

    MaterialProperty material_prop_;

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