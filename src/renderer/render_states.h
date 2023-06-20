#pragma once

#include "core/glm_inc.h"

namespace MR {

enum DepthFunction {
    DepthFunc_NEVER,
    DepthFunc_LESS,
    DepthFunc_EQUAL,
    DepthFunc_LEQUAL,
    DepthFunc_GREATER,
    DepthFunc_NOTEQUAL,
    DepthFunc_GEQUAL,
    DepthFunc_ALWAYS,
};

enum BlendFactor {
    BlendFactor_ZERO,
    BlendFactor_ONE,
    BlendFactor_SRC_COLOR,
    BlendFactor_SRC_ALPHA,
    BlendFactor_DST_COLOR,
    BlendFactor_DST_ALPHA,
    BlendFactor_ONE_MINUS_SRC_COLOR,
    BlendFactor_ONE_MINUS_SRC_ALPHA,
    BlendFactor_ONE_MINUS_DST_COLOR,
    BlendFactor_ONE_MINUS_DST_ALPHA,
};

enum BlendFunction {
    BlendFunc_ADD,
    BlendFunc_SUBTRACT,
    BlendFunc_REVERSE_SUBTRACT,
    BlendFunc_MIN,
    BlendFunc_MAX,
};

enum PolygonMode {
    PolygonMode_POINT,
    PolygonMode_LINE,
    PolygonMode_FILL,
};

struct BlendParameters {
    BlendFunction blend_func_rgb = BlendFunc_ADD;
    BlendFactor blend_src_rgb = BlendFactor_ONE;
    BlendFactor blend_dst_rgb = BlendFactor_ZERO;

    BlendFunction blend_func_alpha = BlendFunc_ADD;
    BlendFactor blend_src_alpha = BlendFactor_ONE;
    BlendFactor blend_dst_alpha = BlendFactor_ZERO;

    void set_blend_factor(BlendFactor src, BlendFactor dst) {
        blend_src_rgb = src;
        blend_src_alpha = src;
        blend_dst_rgb = dst;
        blend_dst_alpha = dst;
    }

    void set_blend_func(BlendFunction func) {
        blend_func_rgb = func;
        blend_func_alpha = func;
    }
};

enum PrimitiveType {
    Primitive_POINT,
    Primitive_LINE,
    Primitive_TRIANGLE,
};

struct RenderStates {
    bool blend = false;
    BlendParameters blend_params;

    bool depth_test = false;
    bool depth_mask = true;
    DepthFunction depth_func = DepthFunc_LESS;

    bool cull_face = false;
    PrimitiveType primitive_type = Primitive_TRIANGLE;
    PolygonMode polygon_mode = PolygonMode_FILL;

    float line_width = 1.f;
};

struct ClearStates {
    bool depth_flag = false;
    bool color_flag = false;
    glm::vec4 clear_color = glm::vec4(0.f);
    float clear_depth = 1.f;
};
}  // namespace MR