#pragma once

#include "material.h"
#include "texture.h"
#include "texture_cube.h"

namespace MR {

class PBRClusterMaterial : public Material {
public:
    void init();

    void fill_unifrom(const Object& obj) override;

private:
    TextureCube* irradiance_map_;
    TextureCube* prefilter_map_;
    Texture* lut_map_;
};

}  // namespace MR