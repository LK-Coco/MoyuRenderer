#pragma once

#include "material.h"

namespace MR {

class BlinnMaterial : public Material {
public:
    void set_diffuse_map(const std::string& file_path);

    void display_ui() override;

    void fill_unifrom() override;
};

}  // namespace MR