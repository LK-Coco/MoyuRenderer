#include <iostream>
#include "rasterizer.h"
#include "scene.h"

namespace MR {

Rasterizer::Rasterizer() : fbo_(FrameBuffer(Scene::width, Scene::height)) {
    // std::string skybox_map_path = "assets/Skybox/Lake/";
}

void Rasterizer::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    draw_dir_light_shadow();

    draw_point_light_shadow();

    draw_depth_pass();

    draw_objects();

    post_process();

    fbo_.bind();
    glViewport(0, 0, Scene::width, Scene::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.9f, 1.0f);

    for (auto& entity : Scene::entities) {
        entity.render();
    }

    fbo_.unbind();
}

void Rasterizer::render_skybox(std::shared_ptr<Skybox>& skybox) {
    fbo_.bind();

    skybox->draw();

    fbo_.unbind();
}

GLuint Rasterizer::get_image_id() const { return fbo_.attach_color_id; }

void Rasterizer::draw_dir_light_shadow() {}

void Rasterizer::draw_point_light_shadow() {}

void Rasterizer::draw_depth_pass() {}

void Rasterizer::draw_objects() {}

void Rasterizer::post_process() {}

}  // namespace MR