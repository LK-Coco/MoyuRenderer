#pragma once

#include "renderer/renderer.h"

namespace MR {

class RendererSoft : Renderer {
public:
    void draw() override;

private:
    void process_clipping();
    void process_face_culling();
    void process_rasterization();

    void draw_line();
};

}  // namespace MR