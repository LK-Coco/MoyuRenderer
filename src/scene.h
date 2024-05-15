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
    static std::uint32_t width;
    static std::uint32_t height;
    static std::string cur_model_path;
    static std::shared_ptr<Skybox> skybox;
    static std::shared_ptr<TextureCube> skybox_tex;
    static std::shared_ptr<Camera> camera;
    static std::shared_ptr<Object> model;
    static std::vector<PointLight> point_light;

    static std::vector<Entity> entities;

    static Entity* selected_entity;

    static DirLight dir_light;

    static void load_json(const char* file_path);
};

}  // namespace MR