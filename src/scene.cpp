#include "scene.h"
#include <memory>
#include "nlohmann_json.h"
#include <fstream>

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

void Scene::load_json(const char* file_path) {
    nlohmann::json scene_json;
    std::ifstream file(file_path);
    file >> scene_json;

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
}

}  // namespace MR