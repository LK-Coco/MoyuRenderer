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
}

}  // namespace MR