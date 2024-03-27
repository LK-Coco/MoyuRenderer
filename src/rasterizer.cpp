#include <iostream>
#include <random>
#include "rasterizer.h"
#include "scene.h"
#include "resources/resources.h"
#include "gpu_data.h"

namespace MR {

Rasterizer::Rasterizer()
    : multi_color_fbo_(Scene::width, Scene::height),
      fbo_(Scene::width, Scene::height),
      gbuffer_fbo_(Scene::width, Scene::height) {
    num_lights_ = Scene::point_light.size();
    for (int i = 0; i < num_lights_; ++i) {
        auto res = Scene::point_light[i].shadow_res;
        point_shadow_fbos_.push_back(PointShadowBufferFBO(res, res));
    }
    for (int i = 0; i < point_shadow_fbos_.size(); ++i) {
        point_shadow_fbos_[i].init();
    }

    multi_color_fbo_.init();
    dir_shadow_fbo_.init();
    for (int i = 0; i < 2; ++i) {
        ping_pong_fbos_.push_back(
            QuadHDRBufferFBO(Scene::width, Scene::height));
    }
    for (int i = 0; i < 2; ++i) {
        ping_pong_fbos_[i].init();
    }
    fbo_.init();

    gbuffer_fbo_.init();

    load_shaders();

    // 随机设置延迟渲染的点光源信息
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist_xz(-3, 3);
    std::uniform_real_distribution<> dist_y(0, 3);
    std::uniform_real_distribution<> dist_c(1.5f, 3.f);
    for (int i = 0; i < DEFERRED_LIGHT_COUNT; i++) {
        vec3 position = vec3(dist_xz(gen), dist_y(gen), dist_xz(gen));
        vec3 color = vec3(dist_c(gen), dist_c(gen), dist_c(gen));
        float light_max = std::fmaxf(std::fmaxf(color.r, color.r), color.b);
        const float constant = 1.0;
        const float linear = 0.7;
        const float quadratic = 1.8;
        float radius =
            (-linear +
             std::sqrtf(linear * linear -
                        4 * quadratic *
                            (constant - (256.0f / 5.0f) * light_max))) /
            (2.0f * quadratic);
        lights_deferred_[i].position = position;
        lights_deferred_[i].color = color;
        lights_deferred_[i].radius = radius;
    }

    std::cout << "init Rasterizer ok" << std::endl;
}

void Rasterizer::render() {
    switch (render_mode) {
        case RenderMode::Forward: forward_render(); break;
        case RenderMode::Deferred: deferred_render(); break;
    }

    // glEnable(GL_DEPTH_TEST);
    // glDepthMask(true);

    // draw_point_light_shadow();

    // multi_sample_fbo_.bind();
    // multi_sample_fbo_.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
    //                         glm::vec3(0.0f));

    // draw_depth_pass();

    // light_assignment();

    // post_process();
}

GLuint Rasterizer::get_image_id() const {
    return multi_color_fbo_.get_attach_color_id(0);
}

void Rasterizer::forward_render() {
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

    light_mesh_shader_.use();
    light_mesh_shader_.set_mat4("projection", Scene::camera->get_projection());
    light_mesh_shader_.set_mat4("view", Scene::camera->get_view_mat());
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.6f, 0, 0));
    model = glm::scale(model, glm::vec3(0.25f));
    light_mesh_shader_.set_mat4("model", model);
    light_mesh_shader_.set_vec3("lightColor", glm::vec3(0, 0, 20));
    light_cube_.render();

    // 关闭面剔除
    glDisable(GL_CULL_FACE);

    skybox_shader_.use();
    skybox_shader_.set_mat4("projection", Scene::camera->get_projection());
    skybox_shader_.set_mat4("view", Scene::camera->get_view_mat());
    skybox_shader_.set_int("environmentMap", 0);

    Scene::skybox->draw();

    multi_color_fbo_.unbind();

    post_process_forward();
}

void Rasterizer::deferred_render() {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    // 启用面剔除
    glEnable(GL_CULL_FACE);

    // 绘制 GBuffer
    gbuffer_fbo_.bind();
    glViewport(0, 0, Scene::width, Scene::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    auto projection = Scene::camera->get_projection();
    auto view = Scene::camera->get_view_mat();

    gbuffer_shader_.use();
    gbuffer_shader_.set_mat4("projection", projection);
    gbuffer_shader_.set_mat4("view", view);
    gbuffer_shader_.set_int("albedoMap", 0);
    gbuffer_shader_.set_int("normalMap", 1);
    gbuffer_shader_.set_int("metallicMap", 2);
    gbuffer_shader_.set_int("roughnessMap", 3);
    gbuffer_shader_.set_int("aoMap", 4);

    for (auto& entity : Scene::entities) {
        gbuffer_shader_.set_mat4("model", entity.obj->get_transform_mat4());

        auto& prop = entity.material_prop;
        if (prop.albedo_map != nullptr) prop.albedo_map->bind(0);
        if (prop.normal_map != nullptr) prop.normal_map->bind(1);
        if (prop.metallic_map != nullptr) prop.metallic_map->bind(2);
        if (prop.roughness_map != nullptr) prop.roughness_map->bind(3);
        if (prop.ao_map != nullptr) prop.ao_map->bind(4);

        entity.obj->render();
    }

    gbuffer_fbo_.unbind();

    // light着色
    multi_color_fbo_.bind();
    glViewport(0, 0, Scene::width, Scene::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    deferred_shading_shader_.use();
    deferred_shading_shader_.set_float("farPlane", Scene::point_light[0].z_far);
    deferred_shading_shader_.set_vec3("camPos", Scene::camera->translation);
    deferred_shading_shader_.set_bool("IBL", false);
    deferred_shading_shader_.set_int("gAbedoM", 0);
    deferred_shading_shader_.set_int("gNormalR", 1);
    deferred_shading_shader_.set_int("gPositionA", 2);
    // deferred_shading_shader_.set_int("irradianceMap", 3);
    // deferred_shading_shader_.set_int("prefilterMap", 4);
    // deferred_shading_shader_.set_int("brdfLUT", 5);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, gbuffer_fbo_.get_attach_color_id(0));
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, gbuffer_fbo_.get_attach_color_id(1));
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, gbuffer_fbo_.get_attach_color_id(2));

    for (int i = 0; i < DEFERRED_LIGHT_COUNT; i++) {
        std::string num = std::to_string(i);

        deferred_shading_shader_.set_vec3("lights[" + num + "].position",
                                          lights_deferred_[i].position);
        deferred_shading_shader_.set_vec3("lights[" + num + "].color",
                                          lights_deferred_[i].color);
        deferred_shading_shader_.set_float("lights[" + num + "].radius",
                                           lights_deferred_[i].radius);
    }

    screen_quad_.render();

    // 拷贝gbuffer的深度图
    gbuffer_fbo_.blit_to(multi_color_fbo_, GL_DEPTH_BUFFER_BIT);
    multi_color_fbo_.bind();
    // 用球显示灯位置
    show_light_shader_.use();
    show_light_shader_.set_mat4("projection", projection);
    show_light_shader_.set_mat4("view", view);
    for (int i = 0; i < DEFERRED_LIGHT_COUNT; i++) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, lights_deferred_[i].position);
        model = glm::scale(model, glm::vec3(0.125f));
        show_light_shader_.set_mat4("model", model);
        show_light_shader_.set_vec3("lightColor", lights_deferred_[i].color);

        light_sphere_.render();
    }

    multi_color_fbo_.unbind();

    // post_process_forward();
}

void Rasterizer::load_shaders() {
    skybox_shader_ = Shader("assets/shaders/skybox/skybox.vs",
                            "assets/shaders/skybox/skybox.fs");
    pbr_shader_ =
        Shader("assets/shaders/pbr/pbr.vs", "assets/shaders/pbr/pbr.fs");

    dir_light_shader_ =
        Shader("assets/shaders/dir_light.vs", "assets/shaders/dir_light.fs");

    blur_shader_ = Shader("assets/shaders/blur.vs", "assets/shaders/blur.fs");

    bloom_shader_ = Shader("assets/shaders/bloom_final.vs",
                           "assets/shaders/bloom_final.fs");

    light_mesh_shader_ =
        Shader("assets/shaders/light_mesh.vs", "assets/shaders/light_mesh.fs");

    point_light_shader_ = Shader("assets/shaders/point_light_shader.vs",
                                 "assets/shaders/point_light_shader.fs",
                                 "assets/shaders/point_light_shader.gs");

    gbuffer_shader_ = Shader("assets/shaders/deferred/gbuffer.vs",
                             "assets/shaders/deferred/gbuffer.fs");
    deferred_shading_shader_ =
        Shader("assets/shaders/deferred/deferred_shading.vs",
               "assets/shaders/deferred/deferred_shading.fs");

    show_light_shader_ =
        Shader("assets/shaders/show_light.vs", "assets/shaders/show_light.fs");
}

void Rasterizer::init_ssbo() {
    size_x_ = (unsigned int)Scene::width / GRID_SIZE_X;
    float z_near = Scene::camera->frustum.near_plane;
    float z_far = Scene::camera->frustum.far_plane;

    // 设置aabb_volume_grid_ssbo_
    // {
    //     glGenBuffers(1, &aabb_volume_grid_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, aabb_volume_grid_ssbo_);

    //     // We generate the buffer but don't populate it yet.
    //     glBufferData(GL_SHADER_STORAGE_BUFFER,
    //                  NUM_CLUSTERS * sizeof(struct VolumeTileAABB), NULL,
    //                  GL_STATIC_COPY);
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
    //     aabb_volume_grid_ssbo_); glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // }

    // 设置screen_to_view_ssbo_
    // {
    //     glGenBuffers(1, &screen_to_view_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, screen_to_view_ssbo_);

    //     // Setting up contents of buffer
    //     screen_to_view.inverse_projection_mat =
    //         glm::inverse(Scene::camera->get_projection());
    //     screen_to_view.tile_sizes[0] = GRID_SIZE_X;
    //     screen_to_view.tile_sizes[1] = GRID_SIZE_Y;
    //     screen_to_view.tile_sizes[2] = GRID_SIZE_Z;
    //     screen_to_view.tile_sizes[3] = size_x_;
    //     screen_to_view.screen_width = Scene::width;
    //     screen_to_view.screen_height = Scene::height;
    //     // Basically reduced a log function into a simple multiplication an
    //     // addition by pre-calculating these
    //     screen_to_view.slice_scaling_factor =
    //         (float)GRID_SIZE_Z / std::log2f(z_far / z_near);
    //     screen_to_view.slice_bias_factor =
    //         -((float)GRID_SIZE_Z * std::log2f(z_near) /
    //           std::log2f(z_far / z_near));

    //     // Generating and copying data to memory in GPU
    //     glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ScreenToView),
    //                  &screen_to_view, GL_STATIC_COPY);
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, screen_to_view_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // }

    // 设置light_ssbo_
    // {
    //     glGenBuffers(1, &light_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_ssbo_);
    //     glBufferData(GL_SHADER_STORAGE_BUFFER,
    //                  MAX_LIGHTS * sizeof(struct GPULight), NULL,
    //                  GL_DYNAMIC_DRAW);

    //     GLint buf_mask = GL_READ_WRITE;

    //     struct GPULight* lights =
    //         (struct GPULight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER,
    //         buf_mask);
    //     PointLight* light;
    //     for (unsigned int i = 0; i < num_lights_; ++i) {
    //         // Fetching the light from the current scene
    //         light = &Scene::point_light[i];
    //         lights[i].position = light->position;
    //         lights[i].color = light->color;
    //         lights[i].enabled = 1;
    //         lights[i].intensity = 1.0f;
    //         lights[i].range = 65.0f;
    //     }
    //     glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, light_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // }

    // 设置light_index_list_ssbo_
    // A list of indices to the lights that are active and intersect with a
    // cluster
    // {
    //     unsigned int totalNumLights =
    //         NUM_CLUSTERS * MAX_LIGHTS_PER_TILE;  // 50 lights per tile max
    //     glGenBuffers(1, &light_index_list_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_index_list_ssbo_);

    //     // We generate the buffer but don't populate it yet.
    //     glBufferData(GL_SHADER_STORAGE_BUFFER,
    //                  totalNumLights * sizeof(unsigned int), NULL,
    //                  GL_STATIC_COPY);
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
    //     light_index_list_ssbo_); glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // }

    // 设置light_grid_ssbo_
    // Every tile takes two unsigned ints one to represent the number of lights
    // in that grid Another to represent the offset to the light index list from
    // where to begin reading light indexes from This implementation is straight
    // up from Olsson paper
    // {
    //     glGenBuffers(1, &light_grid_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_grid_ssbo_);

    //     glBufferData(GL_SHADER_STORAGE_BUFFER,
    //                  NUM_CLUSTERS * 2 * sizeof(unsigned int), NULL,
    //                  GL_STATIC_COPY);
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, light_grid_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // }

    // 设置light_index_global_count_ssbo_
    // Setting up simplest ssbo in the world
    // {
    //     glGenBuffers(1, &light_index_global_count_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER,
    //     light_index_global_count_ssbo_);

    //     // Every tile takes two unsigned ints one to represent the number of
    //     // lights in that grid Another to represent the offset
    //     glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL,
    //                  GL_STATIC_COPY);
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6,
    //                      light_index_global_count_ssbo_);
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // }
}

void Rasterizer::pre_process() {
    build_aabb_grid_comp_shader_.use();
    build_aabb_grid_comp_shader_.set_float("zNear",
                                           Scene::camera->frustum.near_plane);
    build_aabb_grid_comp_shader_.set_float("zFar",
                                           Scene::camera->frustum.far_plane);
    build_aabb_grid_comp_shader_.dispatch(GRID_SIZE_X, GRID_SIZE_Y,
                                          GRID_SIZE_Z);

    glViewport(0, 0, Scene::width, Scene::height);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    draw_point_light_shadow_forward();
}

void Rasterizer::draw_dir_light_shadow_forward() {
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

void Rasterizer::draw_point_light_shadow_forward() {
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

void Rasterizer::post_process_forward() {
    // 模糊
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;

    auto color_1 = multi_color_fbo_.get_attach_color_id(1);

    blur_shader_.use();
    blur_shader_.set_int("image", 0);
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_fbos_[horizontal].fb_id);

        glActiveTexture(GL_TEXTURE0);
        blur_shader_.set_bool("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D,
                      first_iteration
                          ? color_1
                          : ping_pong_fbos_[!horizontal].get_attach_color_id(
                                0));  // bind texture of other
                                      // framebuffer (or scene
        screen_quad_.render();        // if first iteration)

        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }

    // render bloom
    fbo_.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bloom_shader_.use();
    bloom_shader_.set_int("scene", 0);
    bloom_shader_.set_int("bloomBlur", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, multi_color_fbo_.get_attach_color_id(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,
                  ping_pong_fbos_[!horizontal].get_attach_color_id(0));
    bloom_shader_.set_bool("bloom", true);
    bloom_shader_.set_float("exposure", 1.0);
    screen_quad_.render();

    fbo_.unbind();
}

void Rasterizer::draw_depth_pass() {
    glm::mat4 projection = glm::perspective(
        glm::radians(Scene::camera->get_zoom()),
        (float)Scene::width / (float)Scene::height, 0.1f, 100.0f);
    glm::mat4 view = Scene::camera->get_view_mat();

    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 VP = projection * view;

    for (auto& model : Scene::entities) {
        MVP = VP * model.obj->get_transform_mat4();

        depth_shader_.use();
        depth_shader_.set_mat4("MVP", MVP);

        depth_shader_.set_int("albedo", 0);
        model.material_prop.albedo_map->bind(0);

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
    glm::mat4 MVP = glm::mat4(1.0);
    glm::mat4 M = glm::mat4(1.0);
    auto projection = Scene::camera->get_projection();
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
    pbr_cluster_shader_.set_vec3("cameraPos_wS", Scene::camera->translation);
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

    pbr_cluster_shader_.set_bool("IBL", true);

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
        M = model.obj->get_transform_mat4();
        auto& prop = model.material_prop;
        MVP = VP * M;

        pbr_cluster_shader_.set_mat4("MVP", MVP);
        pbr_cluster_shader_.set_mat4("M", M);

        pbr_cluster_shader_.set_int("albedoMap", 0);
        prop.albedo_map->bind(0);

        pbr_cluster_shader_.set_bool("normalMapped", true);
        pbr_cluster_shader_.set_int("normalsMap", 2);
        prop.normal_map->bind(2);

        pbr_cluster_shader_.set_bool("aoMapped", true);
        pbr_cluster_shader_.set_int("lightMap", 3);
        prop.ao_map->bind(3);

        pbr_cluster_shader_.set_int("metalRoughMap", 4);
        prop.metallic_map->bind(4);

        pbr_cluster_shader_.set_int("roughMap", 5);
        prop.roughness_map->bind(5);

        model.render();
    }
}

void Rasterizer::post_process() {}

}  // namespace MR