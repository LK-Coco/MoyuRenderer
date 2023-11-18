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

void BlinnMaterial::set_diffuse_map(const std::string& file_path) {
    diffuse_map_ = Resources::load_texture("texture_diffuse0", file_path);

    shader_->set_int("texture_diffuse0", 0);
}

void BlinnMaterial::display_ui() {
    if (auto ret =
            Utils::imgui_image_button("texture_diffuse0", "texture_diffuse0");
        ret.has_value()) {
    }
}

void BlinnMaterial::fill_unifrom() {
    glm::mat4 projection = glm::perspective(
        glm::radians(60.f), (float)Scene::width / (float)Scene::height, 0.1f,
        100.0f);
    glm::mat4 view = Scene::camera->get_view_mat();

    shader_->use();
    shader_->set_mat4("projection", projection);
    shader_->set_mat4("view", view);
    shader_->set_vec3("light_pos", Scene::point_light[0].position);
    shader_->set_vec3("light_color", Scene::point_light[0].color);
    shader_->set_vec3("view_pos", Scene::camera->position);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader_->set_mat4("model", model);

    diffuse_map_->bind(0);
}

}  // namespace MR