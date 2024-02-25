#include "scene.h"
#include <memory>
#include "nlohmann_json.h"
#include <iostream>
#include <fstream>
#include "object/sphere.h"
#include "object/quad.h"

namespace MR {

int Scene::width = 1280;

int Scene::height = 720;

std::string Scene::cur_model_path = "assets/AfricanHead/african_head.obj";

std::shared_ptr<Skybox> Scene::skybox = nullptr;

std::shared_ptr<Camera> Scene::camera = std::make_shared<Camera>(
    glm::vec3(0.f, 0.f, 3.f), glm::vec3(0, 0, 0), 45.0,
    (float)Scene::width / (float)Scene::height, 1, 1000);

std::shared_ptr<Object> Scene::model = nullptr;

std::vector<PointLight> Scene::point_light;

std::vector<Entity> Scene::entities;

DirLight Scene::dir_light;

Entity get_entity(const std::string& name) {
    if (name == "sphere") {
        return Entity(std::make_shared<Sphere>());
    } else if (name == "quad") {
        return Entity(std::make_shared<Quad>());
    }

    return Entity(std::make_shared<Sphere>());
}

void Scene::load_json(const char* file_path) {
    nlohmann::json scene_json;
    std::ifstream file(file_path);
    file >> scene_json;

    // Models
    nlohmann::json json_models = scene_json["models"];
    unsigned int models_size = json_models.size();
    for (int i = 0; i < models_size; ++i) {
        nlohmann::json json_model = json_models[i];

        auto entity = get_entity(json_model["mesh"]);
        nlohmann::json json_pos = json_model["position"];
        entity.obj->position = glm::vec3((float)json_pos[0], (float)json_pos[1],
                                         (float)json_pos[2]);
        nlohmann::json json_scale = json_model["scale"];
        entity.obj->scale = glm::vec3(
            (float)json_scale[0], (float)json_scale[1], (float)json_scale[2]);
        nlohmann::json json_rotate = json_model["rotation_axis"];
        entity.obj->rotation_axis =
            glm::vec3((float)json_rotate[0], (float)json_rotate[1],
                      (float)json_rotate[2]);
        entity.obj->angle = glm::radians((float)json_model["angle"]);

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