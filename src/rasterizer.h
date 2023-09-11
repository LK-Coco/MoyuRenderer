#pragma once
#include "renderer.h"
#include "skybox.h"

namespace MR {

class Rasterizer : public Renderer {
public:
    Rasterizer();

    void render(std::shared_ptr<Object>& object,
                std::shared_ptr<Shader>& shader) override;

    void render_skybox() override;

    GLuint get_image_id() const override;

private:
    GLuint fbo_;
    GLuint texture_id_;
    Skybox skybox_;
};

}  // namespace MR