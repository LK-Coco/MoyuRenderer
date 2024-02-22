#include <iostream>
#include "rasterizer.h"
#include "scene.h"
#include "resources/resources.h"
#include "gpu_data.h"

namespace MR {

Rasterizer::Rasterizer()
    : fbo_(FrameBuffer(Scene::width, Scene::height)),
      multi_sample_fbo_(Scene::width, Scene::height) {
    num_lights_ = Scene::point_light.size();
    point_shadow_fbos_ = new PointShadowBufferFBO[num_lights_];

    load_shaders();

    init_ssbo();

    pre_process();

    std::cout << "init Rasterizer ok" << std::endl;
}

void Rasterizer::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    dir_shadow_fbo_.bind();
    dir_shadow_fbo_.clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
    draw_dir_light_shadow();

    draw_point_light_shadow();

    // multi_sample_fbo_.bind();
    // multi_sample_fbo_.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
    //                         glm::vec3(0.0f));

    // draw_depth_pass();

    // light_assignment();

    // post_process();

    fbo_.bind();
    glViewport(0, 0, Scene::width, Scene::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.9f, 1.0f);

    draw_objects();

    // for (auto& entity : Scene::entities) {
    //     entity.render(true);
    // }

    fbo_.unbind();
}

void Rasterizer::render_skybox(std::shared_ptr<Skybox>& skybox) {
    fbo_.bind();

    skybox->draw();

    fbo_.unbind();
}

GLuint Rasterizer::get_image_id() const { return fbo_.attach_color_id; }

void Rasterizer::load_shaders() {
    dir_light_shader_ =
        Shader("assets/shaders/dir_light.vs", "assets/shaders/dir_light.fs");

    depth_shader_ =
        Shader("assets/shaders/depth_pass.vs", "assets/shaders/depth_pass.fs");

    cluster_cull_light_shader_ =
        Shader("assets/shaders/cluster_cull_light_shader.comp");

    pbr_cluster_shader_ = Shader("assets/shaders/pbr_cluster_shader.vs",
                                 "assets/shaders/pbr_cluster_shader.fs");

    point_light_shader_ = Shader("assets/shaders/point_light_shader.vs",
                                 "assets/shaders/point_light_shader.fs",
                                 "assets/shaders/point_light_shader.gs");
}

void Rasterizer::init_ssbo() {
    size_x_ = (unsigned int)Scene::width / GRID_SIZE_X;
    float z_near = Scene::camera->frustum.near_plane;
    float z_far = Scene::camera->frustum.far_plane;

    // 设置aabb_volume_grid_ssbo_
    {
        glGenBuffers(1, &aabb_volume_grid_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, aabb_volume_grid_ssbo_);

        // We generate the buffer but don't populate it yet.
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     NUM_CLUSTERS * sizeof(struct VolumeTileAABB), NULL,
                     GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, aabb_volume_grid_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // 设置screen_to_view_ssbo_
    {
        glGenBuffers(1, &screen_to_view_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, screen_to_view_ssbo_);

        // Setting up contents of buffer
        screen_to_view.inverse_projection_mat =
            glm::inverse(Scene::camera->get_projection());
        screen_to_view.tile_sizes[0] = GRID_SIZE_X;
        screen_to_view.tile_sizes[1] = GRID_SIZE_Y;
        screen_to_view.tile_sizes[2] = GRID_SIZE_Z;
        screen_to_view.tile_sizes[3] = size_x_;
        screen_to_view.screen_width = Scene::width;
        screen_to_view.screen_height = Scene::height;
        // Basically reduced a log function into a simple multiplication an
        // addition by pre-calculating these
        screen_to_view.slice_scaling_factor =
            (float)GRID_SIZE_Z / std::log2f(z_far / z_near);
        screen_to_view.slice_bias_factor =
            -((float)GRID_SIZE_Z * std::log2f(z_near) /
              std::log2f(z_far / z_near));

        // Generating and copying data to memory in GPU
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ScreenToView),
                     &screen_to_view, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, screen_to_view_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // 设置light_ssbo_
    {
        glGenBuffers(1, &light_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_ssbo_);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     MAX_LIGHTS * sizeof(struct GPULight), NULL,
                     GL_DYNAMIC_DRAW);

        GLint buf_mask = GL_READ_WRITE;

        struct GPULight* lights =
            (struct GPULight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, buf_mask);
        PointLight* light;
        for (unsigned int i = 0; i < num_lights_; ++i) {
            // Fetching the light from the current scene
            light = &Scene::point_light[i];
            lights[i].position = glm::vec4(light->position, 1.0f);
            lights[i].color = glm::vec4(light->color, 1.0f);
            lights[i].enabled = 1;
            lights[i].intensity = 1.0f;
            lights[i].range = 65.0f;
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, light_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // 设置light_index_list_ssbo_
    // A list of indices to the lights that are active and intersect with a
    // cluster
    {
        unsigned int totalNumLights =
            NUM_CLUSTERS * MAX_LIGHTS_PER_TILE;  // 50 lights per tile max
        glGenBuffers(1, &light_index_list_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_index_list_ssbo_);

        // We generate the buffer but don't populate it yet.
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     totalNumLights * sizeof(unsigned int), NULL,
                     GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, light_index_list_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // 设置light_grid_ssbo_
    // Every tile takes two unsigned ints one to represent the number of lights
    // in that grid Another to represent the offset to the light index list from
    // where to begin reading light indexes from This implementation is straight
    // up from Olsson paper
    {
        glGenBuffers(1, &light_grid_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_grid_ssbo_);

        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     NUM_CLUSTERS * 2 * sizeof(unsigned int), NULL,
                     GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, light_grid_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // 设置light_index_global_count_ssbo_
    // Setting up simplest ssbo in the world
    {
        glGenBuffers(1, &light_index_global_count_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_index_global_count_ssbo_);

        // Every tile takes two unsigned ints one to represent the number of
        // lights in that grid Another to represent the offset
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL,
                     GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6,
                         light_index_global_count_ssbo_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
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

    draw_point_light_shadow();
}

void Rasterizer::draw_dir_light_shadow() {
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
        model.render(false);
    }
}

void Rasterizer::draw_point_light_shadow() {
    // Populating depth cube maps for the point light shadows
    for (unsigned int i = 0; i < num_lights_; ++i) {
        point_shadow_fbos_[i].bind();
        point_shadow_fbos_[i].clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
        PointLight* light = &Scene::point_light[i];
        light->depth_map_tex_id = point_shadow_fbos_[i].attach_depth_id;

        point_light_shader_.use();
        point_light_shader_.set_vec3("lightPos", light->position);
        point_light_shader_.set_float("far_plane", light->z_far);

        glm::mat4 light_matrix, M;
        glm::mat4 shadow_proj = light->shadow_projection_mat;
        for (int face = 0; face < 6; face++) {
            std::string num_s = std::to_string(face);
            light_matrix = shadow_proj * light->look_at_per_face[face];
            point_light_shader_.set_mat4(
                ("shadowMatrices[" + num_s + "]").c_str(), light_matrix);
        }

        for (auto& model : Scene::entities) {
            M = model.obj->get_model_matrix();
            point_light_shader_.set_mat4("M", M);
            model.render(false);
        }
    }
}

void Rasterizer::draw_depth_pass() {
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

        depth_shader_.set_int("albedo", 0);
        material_prop_.albedo_map->bind(0);

        model.render(false);
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
        M = model.obj->get_model_matrix();
        MVP = VP * M;

        pbr_cluster_shader_.set_mat4("MVP", MVP);
        pbr_cluster_shader_.set_mat4("M", M);

        pbr_cluster_shader_.set_int("albedoMap", 0);
        material_prop_.albedo_map->bind(0);

        pbr_cluster_shader_.set_bool("normalMapped", true);
        pbr_cluster_shader_.set_int("normalsMap", 2);
        material_prop_.normal_map->bind(2);

        pbr_cluster_shader_.set_bool("aoMapped", true);
        pbr_cluster_shader_.set_int("lightMap", 3);
        material_prop_.ao_map->bind(3);

        pbr_cluster_shader_.set_int("metalRoughMap", 4);
        material_prop_.metallic_map->bind(4);

        pbr_cluster_shader_.set_int("roughMap", 5);
        material_prop_.roughness_map->bind(5);

        model.render(false);
    }
}

void Rasterizer::post_process() {}

}  // namespace MR