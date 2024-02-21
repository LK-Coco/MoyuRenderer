#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace MR {

struct VolumeTileAABB {
    glm::vec4 min_point;
    glm::vec4 max_point;
};

static VolumeTileAABB frustum;

struct ScreenToView {
    glm::mat4 inverse_projection_mat;
    unsigned int tile_sizes[4];
    unsigned int screen_width;
    unsigned int screen_height;
    float slice_scaling_factor;
    float slice_bias_factor;
};

static ScreenToView screen_to_view;

}  // namespace MR