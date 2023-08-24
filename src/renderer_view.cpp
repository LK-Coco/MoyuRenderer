#include <iostream>

#include "renderer_view.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glad/glad.h"
#include "model.h"
#include "cube.h"
#include "rasterizer.h"

namespace MR {

void RendererView::init(int width, int height) {
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window_ = glfwCreateWindow(width, height, "Render View", nullptr, nullptr);
    if (window_ == nullptr) {
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(window_, this);

    glfwMakeContextCurrent(window_);
    glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto view = static_cast<RendererView*>(glfwGetWindowUserPointer(window));
        view->width_ = width;
        view->height_ = height;
    });
    // glfwSetCursorPosCallback(window_, mouse_call_back);
    // glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetScrollCallback(window_, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto view = static_cast<RendererView*>(glfwGetWindowUserPointer(window));
        view->camera_.on_process_mouse_scroll(yoffset);
    });

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    model_ = std::make_shared<Model>("assets/AfricanHead/african_head.obj");
    shader_ =
        std::make_shared<Shader>("assets/shaders/phone/phone.vs", "assets/shaders/phone/phone.fs");
    renderer_ = std::make_shared<Rasterizer>();

    std::cout << "init view ok!" << std::endl;
}

void RendererView::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        shader_->use();
        glm::mat4 projection = glm::perspective(glm::radians(camera_.get_zoom()),
                                                (float)width_ / (float)height_, 0.1f, 100.0f);
        glm::mat4 view = camera_.get_view_mat();
        shader_->set_mat4("projection", projection);
        shader_->set_mat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(
            model,
            glm::vec3(0.0f, 0.0f, 0.0f));  // translate it down so it's at the center of the scene
        model = glm::scale(
            model,
            glm::vec3(1.0f, 1.0f, 1.0f));  // it's a bit too big for our scene, so scale it down
        shader_->set_mat4("model", model);

        renderer_->render(model_, shader_);

        // 开始ImGui框架新的帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 渲染窗口
        render_main_side(renderer_->get_image_id());
        render_right_side();

        // 渲染ImGui命令
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void RendererView::render_main_side(const GLuint& image) {
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0), ImGuiCond_None);
    ImGui::SetNextWindowSize(ImVec2(GLfloat(width_ * 0.8f), GLfloat(height_)), ImGuiCond_None);
    {
        ImGui::Begin("Main", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_MenuBar);
        ImGui::Image((void*)(intptr_t)image, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
}

void RendererView::render_right_side() {
    int window_width = width_ * 0.2f;
    ImGui::SetNextWindowPos(ImVec2(width_ * 0.8f, 0.0), ImGuiCond_None);
    ImGui::SetNextWindowSize(ImVec2(GLfloat(window_width), GLfloat(height_)), ImGuiCond_None);
    {
        ImGui::Begin("Option", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginTabBar("Overview", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Base")) {
                const char* render_mode[] = {"rasterizer", "ray tracing"};
                ImGui::SetNextItemWidth(window_width * 0.5f);
                if (ImGui::Combo("Render Mode", &cur_render_mode_, render_mode,
                                 IM_ARRAYSIZE(render_mode))) {
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void mouse_call_back(GLFWwindow* window, double xpos, double ypos) {}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

}  // namespace MR