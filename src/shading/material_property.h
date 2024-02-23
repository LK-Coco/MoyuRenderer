#pragma once

#include "resources/resources.h"

namespace MR {

struct MaterialProperty {
    MaterialProperty() {}

    void init_map() {
        albedo_map = Resources::get_texture(albedo_map_path);
        if (albedo_map == nullptr) {
            // 临时
            albedo_map =
                Resources::load_texture(albedo_map_path, albedo_map_path);
        }
        normal_map = Resources::get_texture(normal_map_path);
        if (normal_map == nullptr) {
            normal_map =
                Resources::load_texture(normal_map_path, normal_map_path);
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
    }

    virtual void display_ui() {}

    Texture* albedo_map;
    Texture* normal_map;
    Texture* metallic_map;
    Texture* roughness_map;
    Texture* ao_map;
    TextureCube* irradiance_map;
    TextureCube* prefilter_map;
    Texture* lut_map;

    bool IBL = false;
    std::string albedo_map_path = "assets/gold/albedo.png";
    std::string normal_map_path = "assets/gold/normal.png";
    std::string metallic_map_path = "assets/gold/metallic.png";
    std::string roughness_map_path = "assets/gold/roughness.png";
    std::string ao_map_path = "assets/gold/ao.png";
};

}  // namespace MR