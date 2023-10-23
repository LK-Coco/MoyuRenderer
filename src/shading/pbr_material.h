#pragma once

#include <string.h>
#include "material.h"

namespace MR {

class PBRMaterial : public Material {
public:
    PBRMaterial(const char* vertex_path, const char* fragment_path);

    void set_maps(const std::string& albedo_map_path,
                  const std::string& normal_map_path,
                  const std::string& metallic_map_path,
                  const std::string& roughness_map_path,
                  const std::string& ao_map_path);

    virtual ~PBRMaterial();
};

}  // namespace MR