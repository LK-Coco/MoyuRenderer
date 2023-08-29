#include <iostream>

#include "renderer_view.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include "glad/glad.h"
#include "model.h"
#include "cube.h"
#include "rasterizer.h"

namespace MR {

void RendererView::init(int width, int height) {
    width_ = width;
    height_ = height;
    mouse_xpos_ = width_ * 0.5f;
    mouse_ypos_ = height_ * 0.5f;

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
    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos) {
        auto view = static_cast<RendererView*>(glfwGetWindowUserPointer(window));
        double last_x, last_y;
        glfwGetCursorPos(window, &last_x, &last_y);
        float cur_x = view->width_ - static_cast<float>(last_x);
        float cur_y = view->height_ - static_cast<float>(last_y);
        if (view->mouse_left_clicked_) {
            float delta_x = cur_x - view->mouse_xpos_;
            float delta_y = view->mouse_ypos_ - cur_y;
            view->camera_.on_process_mouse_move(delta_x, delta_y);
        }
        view->mouse_xpos_ = cur_x;
        view->mouse_ypos_ = cur_y;
    });
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            auto view = static_cast<RendererView*>(glfwGetWindowUserPointer(window));
            if (action == GLFW_PRESS) {
                view->mouse_left_clicked_ = true;
            } else if (action == GLFW_RELEASE) {
                view->mouse_left_clicked_ = false;
            }
        }
    });
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
        double current_time = glfwGetTime();
        double deltaTime = current_time - previous_time_;

        // 如果渲染时间小于目标帧时间，延迟剩余时间
        if (deltaTime < target_frame_time) {
            double sleep_time = target_frame_time - deltaTime;
            glfwWaitEventsTimeout(sleep_time - 0.001);
        }
        previous_time_ = current_time;
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
                ImGui::DragFloat("Camera Pos X", &camera_.position.x);
                ImGui::DragFloat("Camera Pos Y", &camera_.position.y);
                ImGui::DragFloat("Camera Pos Z", &camera_.position.z);

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