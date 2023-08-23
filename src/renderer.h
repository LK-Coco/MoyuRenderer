#pragma once
#include <memory>
#include "glad/glad.h"
#include "object.h"

namespace MR {

class Renderer {
public:
    Renderer(){};

    virtual void render(std::shared_ptr<Object>& object){};
    virtual GLuint get_image_id() const { return 0; };
};

}  // namespace MR