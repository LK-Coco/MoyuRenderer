#include "pbr_material.h"
#include "resources/resources.h"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include "scene.h"
#include "utility.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace MR {

void PBRMaterial::set_maps(const std::string& albedo_map_path,
                           const std::string& normal_map_path,
                           const std::string& metallic_map_path,
                           const std::string& roughness_map_path,
                           const std::string& ao_map_path) {
    shader_->use();
    set_albedo_map(albedo_map_path);
    set_normal_map(normal_map_path);
    set_metallic_map(metallic_map_path);
    set_roughness_map(roughness_map_path);
    set_ao_map(ao_map_path);

    // TODO 临时写死
    irradiance_map_ = Resources::get_texture_cube("irradiance_map");
    shader_->set_int("irradianceMap", 5);
    prefilter_map_ = Resources::get_texture_cube("prefilter_map");
    shader_->set_int("prefilterMap", 6);
    lut_map_ = Resources::get_texture("brdf_lut_map");
    shader_->set_int("brdfLUT", 7);
}

void PBRMaterial::set_albedo_map(const std::string& file_path) {
    if (albedo_map_ = Resources::get_texture("albedo_map");
        albedo_map_ != nullptr) {
        Resources::update_texture("albedo_map", file_path);
    } else {
        albedo_map_ = Resources::load_texture("albedo_map", file_path);
    }

    shader_->set_int("albedoMap", 0);
}
void PBRMaterial::set_normal_map(const std::string& file_path) {
    normal_map_ = Resources::load_texture("normal_map", file_path);

    shader_->set_int("normalMap", 1);
}
void PBRMaterial::set_metallic_map(const std::string& file_path) {
    metallic_map_ = Resources::load_texture("metallic_map", file_path);

    shader_->set_int("metallicMap", 2);
}
void PBRMaterial::set_roughness_map(const std::string& file_path) {
    roughness_map_ = Resources::load_texture("roughness_map", file_path);

    shader_->set_int("roughnessMap", 3);
}
void PBRMaterial::set_ao_map(const std::string& file_path) {
    ao_map_ = Resources::load_texture("ao_map", file_path);

    shader_->set_int("aoMap", 4);
}

void PBRMaterial::display_ui() {
    ImGui::Spacing();

    if (auto ret = Utils::imgui_image_button("albedo_map", "albedoMap");
        ret.has_value()) {
        set_albedo_map(ret.value());
    }
    if (auto ret = Utils::imgui_image_button("normal_map", "normalMap");
        ret.has_value()) {
    }
    if (auto ret = Utils::imgui_image_button("metallic_map", "metallicMap");
        ret.has_value()) {
    }
    if (auto ret = Utils::imgui_image_button("roughness_map", "roughnessMap");
        ret.has_value()) {
    }
    if (auto ret = Utils::imgui_image_button("ao_map", "aoMap");
        ret.has_value()) {
    }
}

void PBRMaterial::fill_unifrom(const Object& obj) {
    glm::mat4 projection = glm::perspective(
        glm::radians(Scene::camera->get_zoom()),
        (float)Scene::width / (float)Scene::height, 0.1f, 100.0f);
    glm::mat4 view = Scene::camera->get_view_mat();

    shader_->use();
    shader_->set_mat4("projection", projection);
    shader_->set_mat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, obj.position);

    model = glm::scale(model, obj.scale);
    shader_->set_mat4("model", model);
    shader_->set_mat3("normalMatrix",
                      glm::transpose(glm::inverse(glm::mat3(model))));
    shader_->set_vec3("camPos", Scene::camera->position);

    for (int i = 0; i < 4; i++) {
        shader_->set_vec3("lightPositions[" + std::to_string(i) + "]",
                          Scene::point_light[i].position);
        shader_->set_vec3("lightColors[" + std::to_string(i) + "]",
                          Scene::point_light[i].color);
    }

    // bind texture
    albedo_map_->bind(0);
    normal_map_->bind(1);
    metallic_map_->bind(2);
    roughness_map_->bind(3);
    ao_map_->bind(4);
    irradiance_map_->bind(5);
    prefilter_map_->bind(6);
    lut_map_->bind(7);
}

}  // namespace MR