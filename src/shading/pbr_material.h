#pragma once

#include <string.h>
#include "material.h"
#include "texture.h"
#include "texture_cube.h"

namespace MR {

class PBRMaterial : public Material {
public:
    void set_maps(const std::string& albedo_map_path,
                  const std::string& normal_map_path,
                  const std::string& metallic_map_path,
                  const std::string& roughness_map_path,
                  const std::string& ao_map_path);

    void set_albedo_map(const std::string& file_path);
    void set_normal_map(const std::string& file_path);
    void set_metallic_map(const std::string& file_path);
    void set_roughness_map(const std::string& file_path);
    void set_ao_map(const std::string& file_path);

    void display_ui() override;

    void fill_unifrom() override;

private:
    Texture* albedo_map_;
    Texture* normal_map_;
    Texture* metallic_map_;
    Texture* roughness_map_;
    Texture* ao_map_;
    TextureCube* irradiance_map_;
    TextureCube* prefilter_map_;
    Texture* lut_map_;
};

}  // namespace MR