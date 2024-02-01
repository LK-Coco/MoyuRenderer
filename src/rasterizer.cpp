#include <iostream>
#include "rasterizer.h"
#include "scene.h"

namespace MR {

Rasterizer::Rasterizer() : fbo_(FrameBuffer(Scene::width, Scene::height)) {
    load_shaders();
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

GLuint Rasterizer::get_image_id() const {
    return dir_shadow_fbo_.attach_depth_id;
}

void Rasterizer::load_shaders() {
    dir_light_shader_ =
        Shader("assets/shaders/dir_light.vs", "assets/shaders/dir_light.fs");

    depth_shader_ =
        Shader("assets/shaders/depth_pass.vs", "assets/shaders/depth_pass.fs");
}

void Rasterizer::draw_dir_light_shadow() {
    dir_shadow_fbo_.bind();
    dir_shadow_fbo_.clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));

    float left = Scene::dir_light.ortho_box_size;
    float right = -left;
    float top = left;
    float bottom = -top;
    Scene::dir_light.shadow_projection_mat =
        glm::ortho(left, right, bottom, top, Scene::dir_light.z_near,
                   Scene::dir_light.z_far);
    Scene::dir_light.light_view =
        glm::lookAt(100.0f * -Scene::dir_light.direction,
                    glm::vec3(0.0f, .0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Scene::dir_light.light_space_mat =
        Scene::dir_light.shadow_projection_mat * Scene::dir_light.light_view;

    for (auto& model : Scene::entities) {
        glm::mat4 model_ls =
            Scene::dir_light.light_space_mat * model.obj->get_model_matrix();
        dir_light_shader_.use();
        dir_light_shader_.set_mat4("lightSpaceMatrix", model_ls);
        model.render();
    }
}

void Rasterizer::draw_point_light_shadow() {}

void Rasterizer::draw_depth_pass() {}

void Rasterizer::draw_objects() {}

void Rasterizer::post_process() {}

}  // namespace MR