#pragma once

#include <string.h>
#include "material.h"
#include "texture.h"
#include "texture_cube.h"

namespace MR {

class PBRMaterial : public Material {
public:
    void init() override;

    void set_uniform(std::shared_ptr<Object> obj) override;

public:
    bool IBL = false;
    std::string albedo_map_path = "assets/gold/albedo.png";
    std::string normal_map_path = "assets/gold/normal.png";
    std::string metallic_map_path = "assets/gold/metallic.png";
    std::string roughness_map_path = "assets/gold/roughness.png";
    std::string ao_map_path = "assets/gold/ao.png";

    Texture* albedo_map;
    Texture* normal_map;
    Texture* metallic_map;
    Texture* roughness_map;
    Texture* ao_map;
    TextureCube* irradiance_map;
    TextureCube* prefilter_map;
    Texture* lut_map;
};

}  // namespace MR