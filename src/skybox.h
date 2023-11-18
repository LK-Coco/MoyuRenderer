#pragma once

#include <string>
#include "object/cube.h"
#include "shading/texture_cube.h"

namespace MR {

class Skybox {
public:
    void init();

    void set_map(TextureCube* tex);

    void draw();

private:
    unsigned int skybox_map_id_;
    unsigned int skybox_vao_, skybox_vbo_;
    TextureCube* cube_map_;
    Cube cube_;
};

}  // namespace MR