#include <iostream>
#include "rasterizer.h"
#include "scene.h"

namespace MR {

Rasterizer::Rasterizer()
    : fbo_(FrameBuffer(Scene::width, Scene::height, true)) {
    skybox_ = Skybox();
    std::string skybox_map_path = "assets/Skybox/Lake/";
    skybox_.init(skybox_map_path);
}

void Rasterizer::render(std::shared_ptr<Object>& object,
                        std::shared_ptr<Material>& mat) {
    fbo_.bind();
    glViewport(0, 0, Scene::width, Scene::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.9f, 1.0f);

    object->render(mat);

    fbo_.unbind();
}

void Rasterizer::render_skybox() {
    fbo_.bind();

    skybox_.draw();

    fbo_.unbind();
}

GLuint Rasterizer::get_image_id() const { return fbo_.get_texture_id(); }

}  // namespace MR