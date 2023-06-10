#pragma once

#include <string_view>
#include <vector>
#include "model.h"
#include "light.h"
#include "camera.h"

namespace MR {

class Scene {
public:
    static void parse_from(std::string_view file_path);

private:
    std::vector<Model> models_;
    std::vector<Light> lights_;
    Camera cmera_;
};

}  // namespace MR