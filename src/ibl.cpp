#include "ibl.h"

namespace MR {

Texture IBL::hdri_to_env_cubemap(const Texture& hdri) {}

Texture IBL::env_cubemap_to_irradiance_map(const Texture& cubemap) {}

Texture IBL::env_cubemap_to_prefilter_map(const Texture& cubemap) {}

Texture IBL::clac_brdf_lut() {}

}  // namespace MR