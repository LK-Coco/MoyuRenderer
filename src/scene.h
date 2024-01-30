#pragma once

#include <memory>
#include "camera.h"
#include "object/object.h"
#include "light.h"
#include "skybox.h"
#include "entity.h"

namespace MR {

class Scene {
public:
    static int width;
    static int height;
    static std::string cur_model_path;
    static std::shared_ptr<Skybox> skybox;
    static std::shared_ptr<Camera> camera;
    static std::shared_ptr<Object> model;
    static std::vector<PointLight> point_light;

    static std::vector<Entity> entities;
};

}  // namespace MR