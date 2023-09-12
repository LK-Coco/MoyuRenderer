#include <iostream>

#include "skybox.h"
#include "glad/glad.h"
#include "resources/resources.h"

namespace MR {

void Skybox::init(std::string& path) {
    float skybox_vertices[] = {
        // positions
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    glGenVertexArrays(1, &skybox_vao_);
    glGenBuffers(1, &skybox_vbo_);
    glBindVertexArray(skybox_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);

    cube_map_ = Resources::load_texture_cube("skybox map", path);

    if (cube_map_->face_width != 0) {
        std::cout << "skybox init ok" << std::endl;
    }

    // glGenTextures(1, &skybox_map_id_);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_map_id_);
    // for (auto i = 0; i < 6; ++i) {
    //     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
    //                  cube_map->face_width, cube_map->face_height, 0, GL_RGB,
    //                  GL_FLOAT, nullptr);
    // }
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
    // GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_CUBE_MAP,
    // GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
    // GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_CUBE_MAP,
    // GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_CUBE_MAP,
    // GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Skybox::draw() {
    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(skybox_vao_);
    glActiveTexture(GL_TEXTURE0);
    cube_map_->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

}  // namespace MR