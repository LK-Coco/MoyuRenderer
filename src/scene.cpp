#include "scene.h"
#include <memory>

namespace MR {

int Scene::width = 1280;

int Scene::height = 720;

std::string Scene::cur_model_path = "assets/AfricanHead/african_head.obj";

std::shared_ptr<Camera> Scene::camera =
    std::make_shared<Camera>(glm::vec3(0.f, 0.f, 3.f));

std::shared_ptr<Object> Scene::model = nullptr;

std::shared_ptr<PointLight> Scene::point_light = nullptr;

}  // namespace MR