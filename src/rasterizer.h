#pragma once
#include "renderer.h"

namespace MR {

class Rasterizer : public Renderer {
public:
    Rasterizer();

    void render(std::shared_ptr<Object>& object, std::shared_ptr<Shader>& shader) override;
    GLuint get_image_id() const override;

private:
    GLuint fbo_;
    GLuint texture_id_;
};

}  // namespace MR