#pragma once

#include "base.h"
#include "glm_inc.h"
#include "color.h"

namespace MR {

class Triangle {
public:
    Triangle();

    void set_vertex(int index, glm::vec4 vertex);
    void set_normal(int index, glm::vec3 n);
    void set_color(int index, Color c);
    void set_tex_coord(int index, glm::vec2 uv);

    glm::vec4 v[3];
    Color color[3];
    glm::vec2 tex_coords[3];
    glm::vec3 normal[3];
};

}  // namespace MR