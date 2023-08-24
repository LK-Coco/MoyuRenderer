#pragma once

#include <string>
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "texture.h"
#include "shader.h"

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
    ~Mesh() = default;

    void draw(std::shared_ptr<Shader>& shader);
    void init();

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;

    unsigned int vao_;
    unsigned int vbo_;
    unsigned int ebo_;
};

}  // namespace MR