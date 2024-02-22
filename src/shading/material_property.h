#pragma once

#include "resources/resources.h"

namespace MR {

struct MaterialProperty {
    MaterialProperty() {
        albedo_map = Resources::get_texture("albedo_map");
        if (albedo_map == nullptr) {
            // 临时
            albedo_map =
                Resources::load_texture("albedo_map", "assets/gold/albedo.png");
        }
        normal_map = Resources::get_texture("normal_map");
        if (normal_map == nullptr) {
            normal_map =
                Resources::load_texture("normal_map", "assets/gold/normal.png");
        }
        metallic_map = Resources::get_texture("metallic_map");
        if (metallic_map == nullptr) {
            metallic_map = Resources::load_texture("metallic_map",
                                                   "assets/gold/metallic.png");
        }
        roughness_map = Resources::get_texture("roughness_map");
        if (roughness_map == nullptr) {
            roughness_map = Resources::load_texture(
                "roughness_map", "assets/gold/roughness.png");
        }
        ao_map = Resources::get_texture("ao_map");
        if (ao_map == nullptr) {
            ao_map = Resources::load_texture("ao_map", "assets/gold/ao.png");
        }
    }

    virtual void display_ui() {}

    Texture* albedo_map;
    Texture* normal_map;
    Texture* metallic_map;
    Texture* roughness_map;
    Texture* ao_map;
};

}  // namespace MR