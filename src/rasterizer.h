#pragma once
#include "renderer.h"
#include "skybox.h"
#include "frame_buffer.h"

namespace MR {

class Rasterizer : public Renderer {
public:
    Rasterizer();

    void render(std::shared_ptr<Object>& object,
                std::shared_ptr<Material>& mat) override;

    void render_skybox() override;

    GLuint get_image_id() const override;

private:
    FrameBuffer fbo_;
    Skybox skybox_;
};

}  // namespace MR