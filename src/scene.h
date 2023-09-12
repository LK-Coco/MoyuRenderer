#pragma once

#include <memory>
#include "camera.h"
#include "object.h"

namespace MR {

class Scene {
public:
    static int width;
    static int height;
    static std::shared_ptr<Camera> camera;
    static std::shared_ptr<Object> model;
};

}  // namespace MR