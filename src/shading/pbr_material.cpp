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

void PBRMaterial::init() {
    shader_ = Shader("assets/shaders/pbr/pbr.vs", "assets/shaders/pbr/pbr.fs");

    albedo_map = Resources::get_texture(albedo_map_path);
    if (albedo_map == nullptr) {
        albedo_map = Resources::load_texture(albedo_map_path, albedo_map_path);
    }
    normal_map = Resources::get_texture(normal_map_path);
    if (normal_map == nullptr) {
        normal_map = Resources::load_texture(normal_map_path, normal_map_path);
    }
    metallic_map = Resources::get_texture(metallic_map_path);
    if (metallic_map == nullptr) {
        metallic_map =
            Resources::load_texture(metallic_map_path, metallic_map_path);
    }
    roughness_map = Resources::get_texture(roughness_map_path);
    if (roughness_map == nullptr) {
        roughness_map =
            Resources::load_texture(roughness_map_path, roughness_map_path);
    }
    ao_map = Resources::get_texture(ao_map_path);
    if (ao_map == nullptr) {
        ao_map = Resources::load_texture(ao_map_path, ao_map_path);
    }

    irradiance_map = Resources::get_texture_cube("irradiance_map");
    prefilter_map = Resources::get_texture_cube("prefilter_map");
    lut_map = Resources::get_texture("brdf_lut_map");

    shader_.use();
    shader_.set_int("albedoMap", 0);
    shader_.set_int("normalMap", 1);
    shader_.set_int("metallicMap", 2);
    shader_.set_int("roughnessMap", 3);
    shader_.set_int("aoMap", 4);
    shader_.set_int("irradianceMap", 5);
    shader_.set_int("prefilterMap", 6);
    shader_.set_int("brdfLUT", 7);
    shader_.set_int("dirShadowMap", 8);
}

void PBRMaterial::set_uniform(std::shared_ptr<Object> obj) {
    auto num_lights = Scene::point_light.size();

    shader_.use();
    shader_.set_bool("IBL", IBL);
    auto model = obj->get_transform_mat4();
    shader_.set_mat4("model", model);
    shader_.set_mat3("normalMatrix",
                     glm::transpose(glm::inverse(glm::mat3(model))));

    shader_.set_mat4("lightSpaceMatrix", Scene::dir_light.light_space_mat);
    shader_.set_mat4("projection", Scene::camera->get_projection());
    shader_.set_mat4("view", Scene::camera->get_view_mat());
    shader_.set_vec3("camPos", Scene::camera->translation);
    shader_.set_vec3("dirLight.direction", Scene::dir_light.direction);
    shader_.set_vec3("dirLight.color", Scene::dir_light.color);

    shader_.set_int("pointLightCount", num_lights);
    shader_.set_float("farPlane",
                      Scene::point_light[0].z_far);  // TODO 修正

    if (albedo_map != nullptr) albedo_map->bind(0);
    if (normal_map != nullptr) normal_map->bind(1);
    if (metallic_map != nullptr) metallic_map->bind(2);
    if (roughness_map != nullptr) roughness_map->bind(3);
    if (ao_map != nullptr) ao_map->bind(4);
    if (irradiance_map != nullptr) irradiance_map->bind(5);
    if (prefilter_map != nullptr) prefilter_map->bind(6);
    if (lut_map != nullptr) lut_map->bind(7);
    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, Scene::dir_light.depth_map_tex_id);

    for (int i = 0; i < num_lights; ++i) {
        auto& light = Scene::point_light[i];
        std::string num = std::to_string(i);

        shader_.set_vec3(("pointLightPosition[" + num + "]").c_str(),
                         light.position);
        shader_.set_vec3(("pointLightColor[" + num + "]").c_str(), light.color);

        shader_.set_int(("pointShadowMaps[" + num + "]").c_str(), 9 + i);
        glActiveTexture(GL_TEXTURE0 + 9 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light.depth_map_tex_id);
    }
}

}  // namespace MR