#include "quad.h"

namespace MR {

Quad::Quad() {
    float quad_vertices[] = {
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  // 右上角
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // 左下角
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f   // 左上角
    };

    unsigned int indices[] = {
        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };
    // setup plane VAO
    unsigned int vbo, ebo;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Quad::~Quad() { glDeleteVertexArrays(1, &vao_); }

void Quad::render() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                   0);  // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

}  // namespace MR