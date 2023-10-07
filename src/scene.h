#pragma once

#include <memory>
#include "camera.h"
#include "object.h"
#include "light.h"
namespace MR {

class Scene {
public:
    static int width;
    static int height;
    static std::string cur_model_path;
    static std::shared_ptr<Camera> camera;
    static std::shared_ptr<Object> model;
    static std::shared_ptr<PointLight> point_light;
};

}  // namespace MR