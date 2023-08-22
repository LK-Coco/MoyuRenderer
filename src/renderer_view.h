#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "renderer.h"
namespace MR {

class RendererView {
public:
    RendererView(int width, int height) : width_(width), height_(height) { init(width, height); }
    void run();

private:
    void init(int width, int height);
    void render_main_side(const GLuint& image);
    void render_right_side();

    GLFWwindow* window_;
    int width_;
    int height_;

    int cur_render_mode_;

    Renderer renderer_;
};

}  // namespace MR