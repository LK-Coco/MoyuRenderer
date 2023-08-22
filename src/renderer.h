#pragma once
#include "glad/glad.h"

namespace MR {

class Renderer {
public:
    Renderer();

    virtual void render();
    virtual GLuint get_image_id() const;
};

}  // namespace MR