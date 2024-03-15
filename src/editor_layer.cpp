#include "editor_layer.h"
#include "scene.h"
#include "rasterizer.h"
#include <glm/gtc/type_ptr.hpp>
#include "jyu_app/src/application.h"
#include "physics.h"

#include <iostream>

namespace MR {

void EditorLayer::on_start() {
    Scene::load_json("assets/sample_scene.json");
    std::cout << "init scene ok!" << std::endl;

    renderer_ = std::make_shared<Rasterizer>();

    glfwSetWindowSizeCallback(Jyu::Application::get().get_window_handle(),
                              [](GLFWwindow* window, int width, int height) {
                                  glViewport(0, 0, width, height);
                                  Scene::width = width;
                                  Scene::height = height;
                              });

    mouse_pos_x_ = Scene::width * 0.5f;
    mouse_pos_y_ = Scene::height * 0.5f;

    std::cout << "init render view ok!" << std::endl;
}

void EditorLayer::on_update(float dt) {
    auto mouse_pos = Jyu::Input::get_mouse_position();
    float delta_x = mouse_pos_x_ - mouse_pos.x;
    float delta_y = mouse_pos.y - mouse_pos_y_;
    mouse_pos_x_ = mouse_pos.x;
    mouse_pos_y_ = mouse_pos.y;
    Scene::camera->update(dt, delta_x, delta_y);

    if (ImGui::IsMouseClicked(0) && !ImGuizmo::IsUsing() &&
        !ImGuizmo::IsOver()) {
        Ray ray = Scene::camera->screen_point_to_ray(
            glm::vec3(mouse_pos.x, mouse_pos.y, 0));

        HitResult result;
        if (Physics::ray_cast(ray.origin, ray.direction, result)) {
            Scene::selected_entity = result.hit_entity;
        } else {
            Scene::selected_entity = nullptr;
        }
    }

    renderer_->render();
}

void EditorLayer::on_ui_update() {
    // 渲染窗口
    render_main_side(renderer_->get_image_id());
    render_right_side();
}

void EditorLayer::render_main_side(const GLuint& image) {
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
        if (Scene::selected_entity != nullptr) {
            Entity* entity = Scene::selected_entity;
            auto camera_view = Scene::camera->get_view_mat();
            auto camera_proj = Scene::camera->get_projection();
            auto matrix = entity->obj->get_transform_mat4();
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect(window_pos_x, window_pos_y, window_width,
                              window_height);

            ImGuizmo::Manipulate(glm::value_ptr(camera_view),
                                 glm::value_ptr(camera_proj), gizmo_operation_,
                                 gizmo_mode_, glm::value_ptr(matrix));

            if (ImGuizmo::IsUsing()) {
                float matrix_translation[3], matrix_rotation[3],
                    matrix_scale[3];
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(matrix), matrix_translation, matrix_rotation,
                    matrix_scale);
                entity->obj->translation =
                    glm::vec3(matrix_translation[0], matrix_translation[1],
                              matrix_translation[2]);
                entity->obj->rotation = glm::vec3(
                    matrix_rotation[0], matrix_rotation[1], matrix_rotation[2]);
                entity->obj->scale = glm::vec3(matrix_scale[0], matrix_scale[1],
                                               matrix_scale[2]);
            }
        }

        ImGui::End();
    }
}

void EditorLayer::render_right_side() {
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

        ImGui::PushItemWidth(Scene::width * 0.1f);

        if (ImGui::BeginTabBar("Overview", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Base")) {
                render_gizmo_bar();
                // if (ImGui::Button("Select a model")) {
                //     auto ret = Utils::get_file_path("model files\0*.obj\0");
                //     if (ret.has_value()) {
                //         Scene::model = std::make_shared<Model>(ret.value());
                //         Scene::cur_model_path = ret.value();
                //     }
                // }
                // ImGui::TextWrapped("Current Model: %s",
                //                    Scene::cur_model_path.c_str());
                // const char* render_mode[] = {"rasterizer", "ray tracing"};
                // ImGui::SetNextItemWidth(window_width * 0.5f);
                // if (ImGui::Combo("Render Mode", &cur_render_mode_,
                // render_mode,
                //                  IM_ARRAYSIZE(render_mode))) {
                //     // TODO 接入切换渲染模式
                // }
                // const char* material[] = {"blinn", "pbr"};
                // ImGui::SetNextItemWidth(window_width * 0.5f);
                // if (ImGui::Combo("Material", &cur_material_, material,
                //                  IM_ARRAYSIZE(material))) {
                //     // switch_material();
                // }
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

        ImGui::PopItemWidth();

        ImGui::End();
    }
}

void EditorLayer::render_gizmo_bar() {
    if (ImGui::IsKeyPressed(ImGuiKey_T)) gizmo_operation_ = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E)) gizmo_operation_ = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R))  // r Key
        gizmo_operation_ = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate",
                           gizmo_operation_ == ImGuizmo::TRANSLATE))
        gizmo_operation_ = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", gizmo_operation_ == ImGuizmo::ROTATE))
        gizmo_operation_ = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", gizmo_operation_ == ImGuizmo::SCALE))
        gizmo_operation_ = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Universal",
                           gizmo_operation_ == ImGuizmo::UNIVERSAL))
        gizmo_operation_ = ImGuizmo::UNIVERSAL;

    if (gizmo_operation_ != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", gizmo_mode_ == ImGuizmo::LOCAL))
            gizmo_mode_ = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", gizmo_mode_ == ImGuizmo::WORLD))
            gizmo_mode_ = ImGuizmo::WORLD;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_S)) use_snap_ = !use_snap_;
    ImGui::Checkbox("##UseSnap", &use_snap_);
    ImGui::SameLine();
    switch (gizmo_operation_) {
        case ImGuizmo::TRANSLATE: ImGui::InputFloat3("Snap", &snap_[0]); break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap_[0]);
            break;
        case ImGuizmo::SCALE: ImGui::InputFloat("Scale Snap", &snap_[0]); break;
    }
}

}  // namespace MR