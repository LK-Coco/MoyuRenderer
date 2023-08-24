#pragma once
#include <memory>
#include "glad/glad.h"
#include "object.h"
#include "shader.h"

namespace MR {

class Renderer {
public:
    Renderer(){};

    virtual void render(std::shared_ptr<Object>& object, std::shared_ptr<Shader>& shader){};
    virtual GLuint get_image_id() const { return 0; };
};

}  // namespace MR