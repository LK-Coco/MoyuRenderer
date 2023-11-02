#include "pbr_material.h"
#include "resources/resources.h"

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

}  // namespace MR