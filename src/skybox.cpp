#include <iostream>

#include "skybox.h"
#include "glad/glad.h"
#include "resources/resources.h"

namespace MR {

void Skybox::init() {
    // float skybox_vertices[] = {
    //     // positions
    //     -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    //     1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

    //     -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    //     -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

    //     1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

    //     -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

    //     -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

    //     -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    //     1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    // glGenVertexArrays(1, &skybox_vao_);
    // glGenBuffers(1, &skybox_vbo_);
    // glBindVertexArray(skybox_vao_);
    // glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices,
    //              GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
    //                       (void*)0);

    std::cout << "skybox init ok" << std::endl;
}

void Skybox::set_map(TextureCube* tex) { cube_map_ = tex; }

void Skybox::draw() {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    cube_map_->bind(0);

    cube_.render();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

}  // namespace MR