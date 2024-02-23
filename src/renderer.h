#pragma once
#include <memory>
#include "glad/glad.h"
#include "object/object.h"
#include "shading/material.h"
#include "skybox.h"

namespace MR {

class Renderer {
public:
    Renderer(){};

    virtual void render(){};

    virtual GLuint get_image_id() const { return 0; };
};

}  // namespace MR