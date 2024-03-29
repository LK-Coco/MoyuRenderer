#include <iostream>
#include <Windows.h>

#include "glm/fwd.hpp"
#include "renderer_view.h"
#include "scene.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "glad/glad.h"
#include "object/model.h"
#include "object/cube.h"
#include "object/sphere.h"
#include "object/quad.h"
#include "rasterizer.h"
#include "shading/pbr_material.h"
#include "shading/blinn_material.h"
#include "utility.h"
#include "resources/resources.h"
#include "ImGuizmo/ImGuizmo.h"

namespace MR {

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

static const float identityMatrix[16] = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                         0.f, 0.f, 0.f, 0.f, 1.f, 0.f,
                                         0.f, 0.f, 0.f, 1.f};

float objectMatrix[4][16] = {{1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f,
                              1.f, 0.f, 0.f, 0.f, 0.f, 1.f},

                             {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f,
                              1.f, 0.f, 2.f, 0.f, 0.f, 1.f},

                             {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f,
                              1.f, 0.f, 2.f, 0.f, 2.f, 1.f},

                             {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f,
                              1.f, 0.f, 0.f, 0.f, 2.f, 1.f}};

void EditTransform(float* cameraView, float* cameraProjection, float* matrix,
                   bool editTransformDecomposition) {
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = {1.f, 1.f, 1.f};
    static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
    static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    if (editTransformDecomposition) {
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R))  // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate",
                               mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate",
                               mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale",
                               mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Universal",
                               mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
            mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation,
                                              matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(
            matrixTranslation, matrixRotation, matrixScale, matrix);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("Local",
                                   mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World",
                                   mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S)) useSnap = !useSnap;
        ImGui::Checkbox("##UseSnap", &useSnap);
        ImGui::SameLine();

        switch (mCurrentGizmoOperation) {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Snap", &snap[0]);
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Angle Snap", &snap[0]);
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", &snap[0]);
                break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing) {
            ImGui::PushID(3);
            ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
    static ImGuiWindowFlags gizmoWindowFlags = 0;
    if (true) {
        ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
                              (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
        ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y,
                          windowWidth, windowHeight);
        viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
        viewManipulateTop = ImGui::GetWindowPos().y;
        auto window = ImGui::GetCurrentWindow();
        gizmoWindowFlags =
            ImGui::IsWindowHovered() &&
                    ImGui::IsMouseHoveringRect(window->InnerRect.Min,
                                               window->InnerRect.Max)
                ? ImGuiWindowFlags_NoMove
                : 0;
    } else {
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    }

    ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
    ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], 1);
    ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation,
                         mCurrentGizmoMode, matrix, NULL,
                         useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL,
                         boundSizingSnap ? boundsSnap : NULL);

    ImGuizmo::ViewManipulate(
        cameraView, 8.0f, ImVec2(viewManipulateRight - 128, viewManipulateTop),
        ImVec2(128, 128), 0x10101010);

    if (true) {
        ImGui::End();
        ImGui::PopStyleColor(1);
    }
}

void process_input(GLFWwindow* window, double delta_time);

// bool get_file_path(std::string* file_path);

RendererView::RendererView(int width, int height) { init(width, height); }

void RendererView::init(int width, int height) {
    Scene::width = width;
    Scene::height = height;
    mouse_xpos_ = width * 0.5f;
    mouse_ypos_ = height * 0.5f;

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
    glfwSetWindowSizeCallback(window_,
                              [](GLFWwindow* window, int width, int height) {
                                  glViewport(0, 0, width, height);
                                  Scene::width = width;
                                  Scene::height = height;
                              });
    glfwSetCursorPosCallback(
        window_, [](GLFWwindow* window, double xpos, double ypos) {
            auto view =
                static_cast<RendererView*>(glfwGetWindowUserPointer(window));
            double last_x, last_y;
            glfwGetCursorPos(window, &last_x, &last_y);
            float cur_x = Scene::width - static_cast<float>(last_x);
            float cur_y = Scene::height - static_cast<float>(last_y);
            if (view->mouse_left_clicked_) {
                float delta_x = cur_x - view->mouse_xpos_;
                float delta_y = view->mouse_ypos_ - cur_y;
                Scene::camera->on_process_mouse_move(delta_x, delta_y);
            }
            view->mouse_xpos_ = cur_x;
            view->mouse_ypos_ = cur_y;
        });
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button,
                                           int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            auto view =
                static_cast<RendererView*>(glfwGetWindowUserPointer(window));
            if (action == GLFW_PRESS) {
                view->mouse_left_clicked_ = true;
            } else if (action == GLFW_RELEASE) {
                view->mouse_left_clicked_ = false;
            }
        }
    });
    glfwSetScrollCallback(
        window_, [](GLFWwindow* window, double xoffset, double yoffset) {
            auto view =
                static_cast<RendererView*>(glfwGetWindowUserPointer(window));
            Scene::camera->on_process_mouse_scroll(yoffset);
        });

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return;
    }
    // glfwSetKeyCallback(window_, )

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    // Scene::model = std::make_shared<Sphere>();
    //  std::make_shared<Model>("assets/AfricanHead/african_head.obj");
    //  std::make_shared<Model>("assets/DamagedHelmet/DamagedHelmet.gltf");
    //  std::make_shared<Model>("assets/Cerberus/Cerberus_LP.FBX");

    // 设置灯光
    // Scene::point_light.push_back(
    //     {glm::vec3(300.0f, 0.0f, 0.0f), glm::vec3(0, 0, 5.0f)});
    // Scene::point_light.push_back(
    //     {glm::vec3(0.0f, 300.0f, 0.0f), glm::vec3(-5.0f, 0, 3.0f)});
    // Scene::point_light.push_back(
    //     {glm::vec3(0.0f, 0.0f, 300.0f), glm::vec3(5.0f, 0, 1.0f)});
    // Scene::point_light.push_back(
    //     {glm::vec3(0.0f, 0.0f, 300.0f), glm::vec3(0, 0, -5.0f)});

    // Scene::dir_light.direction = glm::vec3(0.0f, 5.0f, 1.333f);
    // Scene::dir_light.distance = 10;
    // Scene::dir_light.color = glm::vec3(300, 300, 300);
    // Scene::dir_light.strength = 1;
    // Scene::dir_light.z_near = 1.0;
    // Scene::dir_light.z_far = 700;
    // Scene::dir_light.ortho_box_size = 10;
    // Scene::dir_light.shadow_res = 2048;

    // 设置Material
    cur_material_ = 1;
    // switch_material();

    // Entity sphere{std::make_shared<Sphere>()};
    // Entity quad{std::make_shared<Quad>()};
    // quad.obj->position.y = -1.5f;
    // quad.obj->scale = glm::vec3(3, 3, 1);
    // quad.obj->rotation_axis = glm::vec3(1, 0, 0);
    // quad.obj->angle = glm::radians(90.0f);
    // Scene::entities.push_back(sphere);
    // Scene::entities.push_back(quad);
    Scene::load_json("assets/sample_scene.json");
    std::cout << "init scene ok!" << std::endl;

    renderer_ = std::make_shared<Rasterizer>();

    std::cout << "init render view ok!" << std::endl;
}

void RendererView::run() {
    while (!glfwWindowShouldClose(window_)) {
        double current_time = glfwGetTime();
        double deltaTime = current_time - previous_time_;

        // 如果渲染时间小于目标帧时间，延迟剩余时间target_frame_time_
        if (deltaTime < target_frame_time_) {
            double sleep_time = target_frame_time_ - deltaTime;
            glfwWaitEventsTimeout(sleep_time - 0.001);
        }
        previous_time_ = current_time;

        process_input(window_, target_frame_time_);

        renderer_->render();

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

        glfwPollEvents();
        glfwSwapBuffers(window_);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void RendererView::load_model(std::string& file_path) {
    // load obj model
    if (Scene::cur_model_path != file_path) {
        Scene::model = std::make_shared<Model>(file_path);
        Scene::cur_model_path = file_path;
    }
    // load meta file
}

void RendererView::render_main_side(const GLuint& image) {
    float window_pos_x = 0.0f;
    float window_pos_y = 0.0f;
    float window_width = Scene::width * 0.8f;
    float window_height = Scene::height * 0.8f;
    ImGui::SetNextWindowPos(ImVec2(window_pos_x, window_pos_y), ImGuiCond_None);
    ImGui::SetNextWindowSize(
        ImVec2(GLfloat(window_width), GLfloat(window_height)), ImGuiCond_None);
    {
        ImGui::Begin("Main", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_MenuBar);
        ImGui::Image((void*)(intptr_t)image, ImGui::GetContentRegionAvail(),
                     ImVec2(0, 1),
                     ImVec2(1,
                            0));  // 设置imgui原点为左下角，与opengl保持一致

        // 渲染Gizmo
        auto camera_view = Scene::camera->get_view_mat();
        auto camera_proj = Scene::camera->get_projection();
        auto matrix = Scene::entities[0].obj->get_transform_mat4();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(window_pos_x, window_pos_y, window_width,
                          window_height);

        ImGuizmo::Manipulate(glm::value_ptr(camera_view),
                             glm::value_ptr(camera_proj), ImGuizmo::TRANSLATE,
                             ImGuizmo::LOCAL, glm::value_ptr(matrix));

        if (ImGuizmo::IsUsing()) {
            Scene::entities[0].obj->translation = glm::vec3(matrix[3]);
        }

        ImGui::End();
    }
}

void RendererView::render_right_side() {
    int window_width = Scene::width * 0.2f;
    ImGui::SetNextWindowPos(ImVec2(Scene::width * 0.8f, 0.0), ImGuiCond_None);
    ImGui::SetNextWindowSize(
        ImVec2(GLfloat(window_width), GLfloat(Scene::height)), ImGuiCond_None);
    {
        ImGui::Begin("Option", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginTabBar("Overview", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Base")) {
                if (ImGui::Button("Select a model")) {
                    auto ret = Utils::get_file_path("model files\0*.obj\0");
                    if (ret.has_value()) {
                        Scene::model = std::make_shared<Model>(ret.value());
                        Scene::cur_model_path = ret.value();
                    }
                }
                ImGui::TextWrapped("Current Model: %s",
                                   Scene::cur_model_path.c_str());
                const char* render_mode[] = {"rasterizer", "ray tracing"};
                ImGui::SetNextItemWidth(window_width * 0.5f);
                if (ImGui::Combo("Render Mode", &cur_render_mode_, render_mode,
                                 IM_ARRAYSIZE(render_mode))) {
                    // TODO 接入切换渲染模式
                }
                const char* material[] = {"blinn", "pbr"};
                ImGui::SetNextItemWidth(window_width * 0.5f);
                if (ImGui::Combo("Material", &cur_material_, material,
                                 IM_ARRAYSIZE(material))) {
                    // switch_material();
                }
                // obj_mat_->display_ui();
                //  ImGui::DragFloat("Camera Pos X",
                //  &Scene::camera->position.x);
                //  ImGui::DragFloat("Camera Pos Y",
                //  &Scene::camera->position.y);
                //  ImGui::DragFloat("Camera Pos Z",
                //  &Scene::camera->position.z);

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}

void RendererView::switch_material() {
    if (cur_material_ == 0) {
        // blinn
        obj_shader_ = std::make_shared<Shader>("assets/shaders/blinn_phong/"
                                               "blinn_phong.vs",
                                               "assets/shaders/blinn_phong/"
                                               "blinn_phong.fs");

        obj_mat_ = std::make_shared<BlinnMaterial>();
        obj_mat_->set_shader(obj_shader_.get());
        BlinnMaterial* mat = (BlinnMaterial*)obj_mat_.get();
        mat->set_diffuse_map("assets/AfricanHead/"
                             "african_head_diffuse.tga");
    } else if (cur_material_ == 1) {
        // pbr
        obj_shader_ = std::make_shared<Shader>("assets/shaders/pbr/pbr.vs",
                                               "assets/shaders/pbr/pbr.fs");

        obj_mat_ = std::make_shared<PBRMaterial>();
        obj_mat_->set_shader(obj_shader_.get());

        auto pbr = (PBRMaterial*)obj_mat_.get();
        // pbr->set_maps("assets/DamagedHelmet/Default_albedo.jpg",
        //               "assets/DamagedHelmet/Default_normal.jpg",
        //               "assets/DamagedHelmet/Default_metalRoughness.jpg",
        //               "assets/DamagedHelmet/Default_metalRoughness.jpg",
        //               "assets/DamagedHelmet/Default_AO.jpg");
        pbr->set_maps("assets/gold/albedo.png", "assets/gold/normal.png",
                      "assets/gold/metallic.png", "assets/gold/roughness.png",
                      "assets/gold/ao.png");
    }
}

void process_input(GLFWwindow* window, double delta_time) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Scene::camera->on_process_keyboard(CameraMovement::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Scene::camera->on_process_keyboard(CameraMovement::BACKWARD,
                                           delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Scene::camera->on_process_keyboard(CameraMovement::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Scene::camera->on_process_keyboard(CameraMovement::RIGHT, delta_time);
}

// bool get_file_path(std::string* file_path) {
//     TCHAR szBuffer[MAX_PATH] = {0};
//     OPENFILENAME ofn = {0};
//     ofn.lStructSize = sizeof(ofn);
//     ofn.hwndOwner = nullptr;
//     ofn.lpstrFilter = "model files\0*.obj\0";  // 要选择的文件后缀
//     ofn.lpstrFile = szBuffer;                  // 存放文件的缓冲区
//     ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
//     ofn.nFilterIndex = 0;
//     ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
//                 OFN_EXPLORER;  // 标志如果是多选要加上OFN_ALLOWMULTISELECT
//     BOOL bSel = GetOpenFileName(&ofn);

//     *file_path = szBuffer;

//     return bSel;
// }

}  // namespace MR