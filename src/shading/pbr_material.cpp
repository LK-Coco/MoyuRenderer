#include "pbr_material.h"
#include "resources/resources.h"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include "scene.h"

namespace MR {

void PBRMaterial::set_maps(const std::string& albedo_map_path,
                           const std::string& normal_map_path,
                           const std::string& metallic_map_path,
                           const std::string& roughness_map_path,
                           const std::string& ao_map_path) {
    auto albedo_map = Resources::load_texture("albedo_map", albedo_map_path);
    auto normal_map = Resources::load_texture("normal_map", normal_map_path);
    auto metallic_map =
        Resources::load_texture("metallic_map", metallic_map_path);
    auto roughness_map =
        Resources::load_texture("roughness_map", roughness_map_path);
    auto ao_map = Resources::load_texture("ao_map", ao_map_path);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedo_map->id);
    glUniform1i(glGetUniformLocation(shader_->get_id(), "albedoMap"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_map->id);
    glUniform1i(glGetUniformLocation(shader_->get_id(), "normalMap"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, metallic_map->id);
    glUniform1i(glGetUniformLocation(shader_->get_id(), "metallicMap"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, roughness_map->id);
    glUniform1i(glGetUniformLocation(shader_->get_id(), "roughnessMap"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ao_map->id);
    glUniform1i(glGetUniformLocation(shader_->get_id(), "aoMap"), 4);
}

void PBRMaterial::display_ui() {}

void PBRMaterial::fill_unifrom() {
    glm::mat4 projection = glm::perspective(
        glm::radians(60.f), (float)Scene::width / (float)Scene::height, 0.1f,
        100.0f);
    glm::mat4 view = Scene::camera->get_view_mat();

    shader_->use();
    shader_->set_mat4("projection", projection);
    shader_->set_mat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader_->set_mat4("model", model);
    shader_->set_mat3("normalMatrix",
                      glm::transpose(glm::inverse(glm::mat3(model))));
    shader_->set_vec3("camPos", Scene::camera->position);
    for (int i = 0; i < 4; i++) {
        shader_->set_vec3("lightPositions[" + std::to_string(i) + "]",
                          Scene::point_light->position);
        shader_->set_vec4("lightColors[" + std::to_string(i) + "]",
                          Scene::point_light->color);
    }
}

}  // namespace MR