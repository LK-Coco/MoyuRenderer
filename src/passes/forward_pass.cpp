#include "forward_pass.h"

#include "scene.h"

namespace MR {

void ForwardPass::init() {
    num_lights_ = Scene::point_light.size();

    pbr_shader_ =
        Shader("assets/shaders/pbr/pbr.vs", "assets/shaders/pbr/pbr.fs");

    dir_light_shader_ =
        Shader("assets/shaders/dir_light.vs", "assets/shaders/dir_light.fs");
    point_light_shader_ = Shader("assets/shaders/point_light_shader.vs",
                                 "assets/shaders/point_light_shader.fs",
                                 "assets/shaders/point_light_shader.gs");

    for (int i = 0; i < num_lights_; ++i) {
        auto res = Scene::point_light[i].shadow_res;
        point_shadow_fbos_.push_back(PointShadowBufferFBO(res, res));
    }
    for (int i = 0; i < point_shadow_fbos_.size(); ++i) {
        point_shadow_fbos_[i].init();
    }

    multi_color_fbo_.init();
    dir_shadow_fbo_.init();
}

void ForwardPass::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    // 启用面剔除
    glEnable(GL_CULL_FACE);
    // 绘制平行光阴影
    dir_shadow_fbo_.bind();
    dir_shadow_fbo_.clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
    draw_dir_light_shadow_forward();
    dir_shadow_fbo_.unbind();

    // 绘制点光源阴影
    draw_point_light_shadow_forward();

    // 绘制物体
    multi_color_fbo_.bind();
    glViewport(0, 0, Scene::width, Scene::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.9f, 1.0f);

    pbr_shader_.use();
    pbr_shader_.set_mat4("lightSpaceMatrix", Scene::dir_light.light_space_mat);
    pbr_shader_.set_mat4("projection", Scene::camera->get_projection());
    pbr_shader_.set_mat4("view", Scene::camera->get_view_mat());
    pbr_shader_.set_vec3("camPos", Scene::camera->translation);
    pbr_shader_.set_vec3("dirLight.direction", Scene::dir_light.direction);
    pbr_shader_.set_vec3("dirLight.color", Scene::dir_light.color);

    pbr_shader_.set_int("pointLightCount", num_lights_);
    pbr_shader_.set_float("farPlane",
                          Scene::point_light[0].z_far);  // TODO 修正

    pbr_shader_.set_int("albedoMap", 0);
    pbr_shader_.set_int("normalMap", 1);
    pbr_shader_.set_int("metallicMap", 2);
    pbr_shader_.set_int("roughnessMap", 3);
    pbr_shader_.set_int("aoMap", 4);
    pbr_shader_.set_int("irradianceMap", 5);
    pbr_shader_.set_int("prefilterMap", 6);
    pbr_shader_.set_int("brdfLUT", 7);
    pbr_shader_.set_int("dirShadowMap", 8);

    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, Scene::dir_light.depth_map_tex_id);

    for (int i = 0; i < num_lights_; ++i) {
        auto& light = Scene::point_light[i];
        std::string num = std::to_string(i);

        pbr_shader_.set_vec3(("pointLightPosition[" + num + "]").c_str(),
                             light.position);
        pbr_shader_.set_vec3(("pointLightColor[" + num + "]").c_str(),
                             light.color);

        pbr_shader_.set_int(("pointShadowMaps[" + num + "]").c_str(), 9 + i);
        glActiveTexture(GL_TEXTURE0 + 9 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light.depth_map_tex_id);
    }

    for (auto& entity : Scene::entities) {
        auto model = entity.obj->get_transform_mat4();
        auto& prop = entity.material_prop;
        pbr_shader_.set_bool("IBL", entity.material_prop.IBL);
        pbr_shader_.set_mat4("model", model);
        pbr_shader_.set_mat3("normalMatrix",
                             glm::transpose(glm::inverse(glm::mat3(model))));

        if (prop.albedo_map != nullptr) prop.albedo_map->bind(0);
        if (prop.normal_map != nullptr) prop.normal_map->bind(1);
        if (prop.metallic_map != nullptr) prop.metallic_map->bind(2);
        if (prop.roughness_map != nullptr) prop.roughness_map->bind(3);
        if (prop.ao_map != nullptr) prop.ao_map->bind(4);
        if (prop.irradiance_map != nullptr) prop.irradiance_map->bind(5);
        if (prop.prefilter_map != nullptr) prop.prefilter_map->bind(6);
        if (prop.lut_map != nullptr) prop.lut_map->bind(7);

        entity.render();
    }
}

void ForwardPass::draw_dir_light_shadow_forward() {
    float left = Scene::dir_light.ortho_box_size;
    float right = -left;
    float top = left;
    float bottom = -top;

    Scene::dir_light.depth_map_tex_id = dir_shadow_fbo_.attach_depth_id;
    Scene::dir_light.shadow_projection_mat =
        glm::ortho(left, right, bottom, top, Scene::dir_light.z_near,
                   Scene::dir_light.z_far);
    Scene::dir_light.light_view =
        glm::lookAt(Scene::dir_light.direction, glm::vec3(0.0f, .0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    Scene::dir_light.light_space_mat =
        Scene::dir_light.shadow_projection_mat * Scene::dir_light.light_view;

    for (auto& model : Scene::entities) {
        glm::mat4 model_ls =
            Scene::dir_light.light_space_mat * model.obj->get_transform_mat4();
        dir_light_shader_.use();
        dir_light_shader_.set_mat4("lightSpaceMatrix", model_ls);
        model.render();
    }
}

void ForwardPass::draw_point_light_shadow_forward() {
    // Populating depth cube maps for the point light shadows
    for (unsigned int i = 0; i < num_lights_; ++i) {
        point_shadow_fbos_[i].bind();
        point_shadow_fbos_[i].clear(GL_DEPTH_BUFFER_BIT, glm::vec3(0.1f));
        PointLight* light = &Scene::point_light[i];
        light->depth_map_tex_id = point_shadow_fbos_[i].attach_depth_id;

        point_light_shader_.use();
        point_light_shader_.set_vec3("lightPos", light->position);
        point_light_shader_.set_float("far_plane", light->z_far);

        glm::mat4 light_matrix, M;
        glm::mat4 shadow_proj = light->shadow_projection_mat;
        for (int face = 0; face < 6; face++) {
            std::string num_s = std::to_string(face);

            point_light_shader_.set_mat4(
                ("shadowMatrices[" + num_s + "]").c_str(),
                light->shadow_transforms[face]);
        }

        for (auto& model : Scene::entities) {
            M = model.obj->get_transform_mat4();
            point_light_shader_.set_mat4("M", M);
            model.render();
        }
        point_shadow_fbos_[i].unbind();
    }
}

}  // namespace MR