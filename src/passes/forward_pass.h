#pragma once

#include "pass.h"

namespace MR {

class ForwardPass : public Pass {
public:
    void init() override;

    void render() override;

    void deinit() override;

private:
    void draw_dir_light_shadow_forward();

    void draw_point_light_shadow_forward();

private:
    Shader dir_light_shader_;
    Shader point_light_shader_;
    Shader pbr_shader_;

    MultiColorFBO multi_color_fbo_;
    DirShadowBufferFBO dir_shadow_fbo_;
    std::vector<PointShadowBufferFBO> point_shadow_fbos_;

    unsigned int num_lights_;
};

}  // namespace MR