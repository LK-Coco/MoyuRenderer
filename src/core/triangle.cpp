#include "triangle.h"

namespace MR {

Triangle::Triangle() {
    v[0] = glm::vec4(0, 0, 0, 1);
    v[1] = glm::vec4(0, 0, 0, 1);
    v[2] = glm::vec4(0, 0, 0, 1);

    color[0] = Color();
    color[1] = Color();
    color[2] = Color();

    tex_coords[0] = glm::vec2(0, 0);
    tex_coords[1] = glm::vec2(0, 0);
    tex_coords[2] = glm::vec2(0, 0);
}

void Triangle::set_vertex(int index, glm::vec4 vertex) { v[index] = vertex; }
void Triangle::set_normal(int index, glm::vec3 n) { normal[index] = n; }
void Triangle::set_color(int index, Color c) { color[index] = c; }
void Triangle::set_tex_coord(int index, glm::vec2 uv) { tex_coords[index] = uv; }

}  // namespace MR