#pragma once

#include <string>
#include "shading/texture_cube.h"

namespace MR {

class Skybox {
public:
    void init(std::string& path);

    void draw();

private:
    unsigned int skybox_map_id_;
    unsigned int skybox_vao_, skybox_vbo_;
    TextureCube* cube_map_;
};

}  // namespace MR