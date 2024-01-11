#pragma once

#include <string>
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "shading/texture.h"
#include "shading/material.h"

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

struct Mesh {
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<Texture> textures);
    ~Mesh() = default;

    void init();

    void draw();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
};

}  // namespace MR