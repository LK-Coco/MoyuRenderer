#include <iostream>
#include "rasterizer.h"
#include "scene.h"
#include "resources/resources.h"

namespace MR {

Rasterizer::Rasterizer()
    : fbo_(FrameBuffer(Scene::width, Scene::height)),
      multi_sample_fbo_(Scene::width, Scene::height) {
    load_shaders();
}

void Rasterizer::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    draw_dir_light_shadow();

    draw_point_light_shadow();

    draw_depth_pass();

    light_assignment();

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
    return multi_sample_fbo_.attach_color_id;
}

void Rasterizer::load_shaders() {
    dir_light_shader_ =
        Shader("assets/shaders/dir_light.vs", "assets/shaders/dir_light.fs");

    depth_shader_ =
        Shader("assets/shaders/depth_pass.vs", "assets/shaders/depth_pass.fs");

    cluster_cull_light_shader_ =
        Shader("assets/shaders/cluster_cull_light_shader.comp");

    pbr_cluster_shader_ = Shader("assets/shaders/pbr_cluster_shader.vs",
                                 "assets/shaders/pbr_cluster_shader.fs");
}

void Rasterizer::init_ssbo() {}

void Rasterizer::pre_process() {}

void Rasterizer::draw_dir_light_shadow() {
    dir_shadow_fbo_.bind();
    dir_shadow_fbo_.clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));

    float left = Scene::dir_light.ortho_box_size;
    float right = -left;
    float top = left;
    float bottom = -top;

    Scene::dir_light.depth_map_tex_id = dir_shadow_fbo_.attach_depth_id;
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

void Rasterizer::draw_depth_pass() {
    multi_sample_fbo_.bind();
    multi_sample_fbo_.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                            glm::vec3(0.0f));

    glm::mat4 projection = glm::perspective(
        glm::radians(Scene::camera->get_zoom()),
        (float)Scene::width / (float)Scene::height, 0.1f, 100.0f);
    glm::mat4 view = Scene::camera->get_view_mat();

    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 VP = projection * view;

    for (auto& model : Scene::entities) {
        MVP = VP * model.obj->get_model_matrix();

        depth_shader_.use();
        depth_shader_.set_mat4("MVP", MVP);

        model.render();
    }
}

void Rasterizer::light_assignment() {
    cluster_cull_light_shader_.use();
    cluster_cull_light_shader_.set_mat4("viewMatrix",
                                        Scene::camera->get_view_mat());
    cluster_cull_light_shader_.dispatch(1, 1, 6);
}

void Rasterizer::draw_objects() {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(false);

    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 M = glm::mat4(1.0);
    auto projection =
        Scene::camera->get_projection(Scene::width, Scene::height);
    auto view = Scene::camera->get_view_mat();
    glm::mat4 VP = projection * view;
    glm::mat4 VPCubeMap = projection * glm::mat4(glm::mat3(view));

    pbr_cluster_shader_.use();
    pbr_cluster_shader_.set_vec3("dirLight.direction",
                                 Scene::dir_light.direction);
    pbr_cluster_shader_.set_bool("slices", false);
    pbr_cluster_shader_.set_vec3(
        "dirLight.color", Scene::dir_light.strength * Scene::dir_light.color);
    pbr_cluster_shader_.set_mat4("lightSpaceMatrix",
                                 Scene::dir_light.light_space_mat);
    pbr_cluster_shader_.set_vec3("cameraPos_wS", Scene::camera->position);
    pbr_cluster_shader_.set_float("zFar", 300.0);
    pbr_cluster_shader_.set_float("zNear", 1.0);

    const unsigned int num_textures = 5;

    auto point_light_count = Scene::point_light.size();
    for (int i = 0; i < point_light_count; i++) {
        auto& light = Scene::point_light[i];
        std::string num = std::to_string(i);

        glActiveTexture(GL_TEXTURE0 + num_textures + i);
        pbr_cluster_shader_.set_int(("depthMaps[" + num + "]").c_str(),
                                    num_textures + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light.depth_map_tex_id);
        pbr_cluster_shader_.set_float("far_plane", light.z_far);
    }

    glActiveTexture(GL_TEXTURE0 + num_textures + point_light_count);
    pbr_cluster_shader_.set_int("shadowMap", num_textures + point_light_count);
    glBindTexture(GL_TEXTURE_2D, Scene::dir_light.depth_map_tex_id);

    auto irradiance_map = Resources::get_texture_cube("irradiance_map");
    auto prefilter_map = Resources::get_texture_cube("prefilter_map");
    auto lut_map = Resources::get_texture("brdf_lut_map");

    pbr_cluster_shader_.set_int("irradianceMap",
                                num_textures + point_light_count + 1);
    irradiance_map->bind(num_textures + point_light_count + 1);

    pbr_cluster_shader_.set_int("prefilterMap",
                                num_textures + point_light_count + 2);
    prefilter_map->bind(num_textures + point_light_count + 2);

    pbr_cluster_shader_.set_int("brdfLUT",
                                num_textures + point_light_count + 3);
    lut_map->bind(num_textures + point_light_count + 3);

    for (auto& model : Scene::entities) {
        M = model.obj->get_model_matrix();
        MVP = VP * M;

        pbr_cluster_shader_.set_mat4("MVP", MVP);
        pbr_cluster_shader_.set_mat4("M", M);

        model.render();
    }
}

void Rasterizer::post_process() {}

}  // namespace MR