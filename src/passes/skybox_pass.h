#pragma once

#include "gl/shader.h"
#include "object/cube.h"

namespace MR {

class SkyboxPass {
public:
    SkyboxPass();

    void render(const glm::mat4& proj, const glm::mat4& view);

private:
    gl::Shader skybox_shader_;
    Cube cube_;
};

}  // namespace MR