#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "texture.h"

namespace MR {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    // int m_BoneIDs[MAX_BONE_INFLUENCE];
    // float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<Texture> textures);
};

}  // namespace MR