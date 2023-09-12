#include "scene.h"

namespace MR {

int Scene::width = 1280;

int Scene::height = 720;

std::shared_ptr<Camera> Scene::camera = std::make_shared<Camera>(
    glm::vec3(0.f, 0.f, 15.f), glm::vec3(0.f, 0.f, 0.f));

std::shared_ptr<Object> Scene::model = nullptr;

}  // namespace MR