#pragma once

#include "material.h"
#include "texture.h"

namespace MR {

class BlinnMaterial : public Material {
public:
    void init() override;

    void set_uniform(std::shared_ptr<Object> obj) override;

public:
    Texture* diffuse_map;
    std::string diffuse_map_path;
};

}  // namespace MR