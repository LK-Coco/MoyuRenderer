#pragma once

#include "shading/texture.h"

namespace MR {

class IBL {
public:
    [[nodiscard]] Texture hdri_to_env_cubemap(const Texture& hdri);

    [[nodiscard]] Texture env_cubemap_to_irradiance_map(const Texture& cubemap);

    [[nodiscard]] Texture env_cubemap_to_prefilter_map(const Texture& cubemap);

    [[nodiscard]] Texture clac_brdf_lut();
};

}  // namespace MR