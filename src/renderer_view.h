#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "renderer.h"
#include "shading/material.h"
#include "camera.h"
#include "skybox.h"

namespace MR {

class RendererView {
public:
    RendererView(int width, int height);
    void run();

private:
    void init(int width, int height);
    void load_model(std::string& file_path);
    void render_main_side(const GLuint& image);
    void render_right_side();

    GLFWwindow* window_;

    bool mouse_left_clicked_ = false;
    float mouse_xpos_ = 0;
    float mouse_ypos_ = 0;

    int cur_render_mode_ = 0;

    std::shared_ptr<Shader> skybox_shader_;

    // Camera camera_{glm::vec3(0.f, 0.f, 15.f), glm::vec3(0.f, 0.f, 0.f)};

    std::shared_ptr<Shader> obj_shader_;
    std::shared_ptr<Material> obj_mat_;
    std::shared_ptr<Renderer> renderer_;

    const int target_frame_rate_ = 60;
    const double target_frame_time_ = 1.0 / target_frame_rate_;
    double previous_time_ = 0;
};

}  // namespace MR