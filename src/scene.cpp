#include "scene.h"
#include <memory>
#include "nlohmann_json.h"
#include <iostream>
#include <fstream>
#include "object/sphere.h"
#include "object/quad.h"
#include "object/model.h"

namespace MR {

std::uint32_t Scene::width = 1280;

std::uint32_t Scene::height = 720;

std::string Scene::cur_model_path = "assets/AfricanHead/african_head.obj";

std::shared_ptr<Skybox> Scene::skybox = nullptr;

std::shared_ptr<Camera> Scene::camera;

std::shared_ptr<Object> Scene::model = nullptr;

std::vector<PointLight> Scene::point_light;

std::vector<Entity> Scene::entities;

Entity* Scene::selected_entity = nullptr;

DirLight Scene::dir_light;

Entity get_entity(const std::string& name) {
    if (name == "sphere") {
        return Entity(std::make_shared<Sphere>());
    } else if (name == "quad") {
        return Entity(std::make_shared<Quad>());
    } else {
        return Entity(std::make_shared<Model>(name));
    }
}

void Scene::load_json(const char* file_path) {
    nlohmann::json scene_json;
    std::ifstream file(file_path);
    file >> scene_json;

    //  width height
    width = (std::uint32_t)scene_json["scene_width"];
    height = (std::uint32_t)scene_json["scene_height"];

    // skybox
    // 预处理hdr环境贴图
    uint32_t skybox_res = (uint32_t)scene_json["sky_box"]["resolution"];
    auto skybox_map = scene_json["sky_box"]["map"];

    auto fbo = CaptureFBO(skybox_res, skybox_res);
    fbo.init();
    // 加载hdr环境贴图
    auto hdr_tex = Resources::load_hdr_texture("hdr_env_map", skybox_map);
    // 将hdr环境贴图转换为环境立方体贴图
    auto env_map_to_cubemap_shader = std::make_shared<Shader>(
        "assets/shaders/pbr/environment_map_to_cubemap.vs",
        "assets/shaders/pbr/environment_map_to_cubemap.fs");
    auto hdr_cubemap = Resources::environment_map_to_cubemap(
        "hdr_cubemap", hdr_tex, env_map_to_cubemap_shader, fbo);

    // 从环境立方体贴图计算出辐照度图
    auto env_cubemap_to_irradiance_map_shader = std::make_shared<Shader>(
        "assets/shaders/pbr/env_cubemap_to_irradiance_map.vs",
        "assets/shaders/pbr/env_cubemap_to_irradiance_map.fs");
    auto irradiance_map = Resources::env_cubemap_to_irradiance_map(
        "irradiance_map", hdr_cubemap, env_cubemap_to_irradiance_map_shader,
        fbo);

    // 从环境立方体贴图计算出预滤波hdr环境贴图
    auto env_cubemap_to_prefilter_map_shader = std::make_shared<Shader>(
        "assets/shaders/pbr/env_cubemap_to_prefilter_map.vs",
        "assets/shaders/pbr/env_cubemap_to_prefilter_map.fs");
    auto prefilter_map = Resources::env_cubemap_to_prefilter_map(
        "prefilter_map", hdr_cubemap, env_cubemap_to_prefilter_map_shader, fbo);

    // 计算出BRDF的LUT图
    auto brdf_lut_shader = std::make_shared<Shader>(
        "assets/shaders/pbr/brdf_lut.vs", "assets/shaders/pbr/brdf_lut.fs");
    auto brdf_lut =
        Resources::clac_brdf_lut("brdf_lut_map", brdf_lut_shader, fbo);

    // 设置天空盒
    skybox = std::make_shared<Skybox>();
    skybox->set_hdr_cube_map(hdr_cubemap);

    // camera
    nlohmann::json json_camera = scene_json["camera"];
    nlohmann::json json_pos = json_camera["position"];
    nlohmann::json json_target = json_camera["target"];

    camera = std::make_shared<Camera>(
        glm::vec3((float)json_pos[0], (float)json_pos[1], (float)json_pos[2]),
        glm::vec3((float)json_target[0], (float)json_target[1],
                  (float)json_target[2]),
        (float)json_camera["fov"], (float)width / (float)height,
        (float)json_camera["near_p"], (float)json_camera["far_p"]);

    // Models
    nlohmann::json json_models = scene_json["models"];
    unsigned int models_size = json_models.size();
    for (int i = 0; i < models_size; ++i) {
        nlohmann::json json_model = json_models[i];

        auto entity = get_entity(json_model["mesh"]);
        nlohmann::json json_pos = json_model["position"];
        entity.obj->translation = glm::vec3(
            (float)json_pos[0], (float)json_pos[1], (float)json_pos[2]);
        nlohmann::json json_scale = json_model["scale"];
        entity.obj->scale = glm::vec3(
            (float)json_scale[0], (float)json_scale[1], (float)json_scale[2]);
        nlohmann::json json_rotate = json_model["rotation"];
        entity.obj->rotation =
            glm::vec3((float)json_rotate[0], (float)json_rotate[1],
                      (float)json_rotate[2]);

        nlohmann::json json_prop = json_model["material_prop"];
        entity.material_prop.IBL = json_prop["IBL"];
        entity.material_prop.albedo_map_path = json_prop["albedo_map_path"];
        entity.material_prop.normal_map_path = json_prop["normal_map_path"];
        entity.material_prop.metallic_map_path = json_prop["metallic_map_path"];
        entity.material_prop.roughness_map_path =
            json_prop["roughness_map_path"];
        entity.material_prop.ao_map_path = json_prop["ao_map_path"];
        entity.material_prop.init_map();

        entities.push_back(entity);
    }

    // 平行光
    nlohmann::json json_dir_light = scene_json["dir_light"];
    nlohmann::json json_direction = json_dir_light["dir"];
    dir_light.direction =
        glm::vec3((float)json_direction[0], (float)json_direction[1],
                  (float)json_direction[2]);
    dir_light.distance = (float)json_dir_light["distance"];
    nlohmann::json json_color = json_dir_light["color"];
    dir_light.color = glm::vec3((float)json_color[0], (float)json_color[1],
                                (float)json_color[2]);
    dir_light.strength = (float)json_dir_light["strength"];
    dir_light.z_near = (float)json_dir_light["z_near"];
    dir_light.z_far = (float)json_dir_light["z_far"];
    dir_light.ortho_box_size = (float)json_dir_light["ortho_size"];
    dir_light.shadow_res = (unsigned int)json_dir_light["shadow_res"];

    // 点光源
    nlohmann::json json_point_lights = scene_json["point_lights"];
    uint32_t point_lights_count = json_point_lights.size();
    for (uint32_t i = 0; i < point_lights_count; ++i) {
        nlohmann::json json_point_light = json_point_lights[i];
        nlohmann::json json_light_pos = json_point_light["position"];
        nlohmann::json json_color = json_point_light["color"];
        glm::vec3 pos((float)json_light_pos[0], (float)json_light_pos[1],
                      (float)json_light_pos[2]);
        glm::vec3 color((float)json_color[0], (float)json_color[1],
                        (float)json_color[2]);
        PointLight light(color, pos);
        light.strength = json_point_light["strength"];
        light.z_near = json_point_light["z_near"];
        light.z_far = json_point_light["z_far"];
        light.shadow_res = json_point_light["shadow_res"];

        point_light.push_back(light);
    }
}

}  // namespace MR