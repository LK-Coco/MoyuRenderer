#include <iostream>
#include "rasterizer.h"
#include "scene.h"

namespace MR {

Rasterizer::Rasterizer() {
    fbo_color_tex_.generate_2d(Scene::width, Scene::height, GL_RGB, GL_RGB,
                               GL_UNSIGNED_BYTE, NULL);

    rbo_.set_storage(GL_DEPTH_ATTACHMENT, Scene::width, Scene::height);

    fbo_.attach_texture(GL_COLOR_ATTACHMENT0, fbo_color_tex_);
}

void Rasterizer::render(std::shared_ptr<Object>& object,
                        std::shared_ptr<Material>& mat) {
    fbo_.bind();
    glViewport(0, 0, Scene::width, Scene::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.9f, 1.0f);

    object->render();

    fbo_.unbind();
}

void Rasterizer::render_skybox(std::shared_ptr<Skybox>& skybox) {
    fbo_.bind();

    skybox->draw();

    fbo_.unbind();
}

GLuint Rasterizer::get_image_id() const { return fbo_color_tex_.id; }

}  // namespace MR