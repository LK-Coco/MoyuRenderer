#pragma once

#include <string.h>
#include "material.h"

namespace MR {

class PBRMaterial : public Material {
public:
    void set_maps(const std::string& albedo_map_path,
                  const std::string& normal_map_path,
                  const std::string& metallic_map_path,
                  const std::string& roughness_map_path,
                  const std::string& ao_map_path);

    void display_ui() override;

    void fill_unifrom() override;
};

}  // namespace MR