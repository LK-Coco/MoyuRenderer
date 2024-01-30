#pragma once

#include "material.h"
#include "texture.h"

namespace MR {

class BlinnMaterial : public Material {
public:
    void set_diffuse_map(const std::string& file_path);

    void display_ui() override;

    void fill_unifrom(const Object& obj) override;

private:
    Texture* diffuse_map_;
};

}  // namespace MR