#include "renderer_view.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glad/glad.h"

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
    // glfwSetScrollCallback(window_, scroll_callback);

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
}

void RendererView::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        // 开始ImGui框架新的帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 渲染窗口
        render_main_side();
        render_right_side();

        // 渲染ImGui命令
        ImGui::Render();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void RendererView::render_main_side() {
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0), ImGuiCond_None);
    ImGui::SetNextWindowSize(ImVec2(GLfloat(width_ * 0.8f), GLfloat(height_)), ImGuiCond_None);
    {
        ImGui::Begin("Main", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_MenuBar);
        ImGui::Image((void*)(intptr_t)0, ImGui::GetContentRegionAvail(), ImVec2(0, 1),
                     ImVec2(1, 0));
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
                if (ImGui::Combo("Select Item", &cur_render_mode_, render_mode,
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