#include "blinn_material.h"
#include "resources/resources.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utility.h"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include "scene.h"

namespace MR {

void BlinnMaterial::init() {
    diffuse_map = Resources::get_texture(diffuse_map_path);
    if (diffuse_map == nullptr) {
        diffuse_map =
            Resources::load_texture(diffuse_map_path, diffuse_map_path);
    }

    shader_ = Shader("assets/shaders/blinn_phong/blinn_phong.vs",
                     "assets/shaders/blinn_phong/blinn_phong.vs");

    shader_.use();
    shader_.set_int("texture_diffuse0", 0);
}

void BlinnMaterial::set_uniform(std::shared_ptr<Object> obj) {
    glm::mat4 projection = glm::perspective(
        glm::radians(60.f), (float)Scene::width / (float)Scene::height, 0.1f,
        100.0f);
    glm::mat4 view = Scene::camera->get_view_mat();

    shader_.use();
    shader_.set_mat4("projection", projection);
    shader_.set_mat4("view", view);
    shader_.set_vec3("light_pos", Scene::point_light[0].position);
    shader_.set_vec3("light_color", Scene::point_light[0].color);
    shader_.set_vec3("view_pos", Scene::camera->translation);
    glm::mat4 model = obj->get_transform_mat4();
    shader_.set_mat4("model", model);

    if (diffuse_map != nullptr) diffuse_map->bind(0);
}

}  // namespace MR