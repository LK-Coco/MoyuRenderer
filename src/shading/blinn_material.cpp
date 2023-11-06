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
    auto diffuse_map = Resources::load_texture("texture_diffuse0", file_path);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map->id);
    glUniform1i(glGetUniformLocation(shader_->get_id(), "texture_diffuse0"), 0);
}

void BlinnMaterial::display_ui() {
    ImVec2 size{100, 100};
    auto map = Resources::get_texture("texture_diffuse0");
    ImTextureID id = map == nullptr ? 0 : (ImTextureID)(map->id);
    if (ImGui::ImageButton(id, size)) {
        std::string tex_path;
        if (Utils::get_file_path(&tex_path, "image files\0*.jpg\0")) {
            set_diffuse_map(tex_path);
        }
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
    shader_->set_vec3("light_pos", Scene::point_light->position);
    shader_->set_vec4("light_color", Scene::point_light->color);
    shader_->set_vec3("view_pos", Scene::camera->position);
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    // shader_->set_mat4("model", model);
}

}  // namespace MR