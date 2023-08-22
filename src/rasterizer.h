#pragma once
#include "renderer.h"

namespace MR {

class Rasterizer : Renderer {
public:
    void render() override;
    GLuint get_image_id() const override;
};

}  // namespace MR