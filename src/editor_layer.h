#pragma once

#include "renderer.h"
#include "imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include "jyu_app/src/layer.h"

namespace MR {

class EditorLayer : public Jyu::Layer {
public:
    void on_start() override;

    void on_update(float dt) override;

    void on_ui_update() override;

private:
    void render_main_side(const GLuint& image);
    void render_right_side();

    void render_gizmo_bar();

    std::shared_ptr<Renderer> renderer_;

    ImGuizmo::OPERATION gizmo_operation_ = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE gizmo_mode_ = ImGuizmo::LOCAL;
    bool use_snap_;
    float snap_[3] = {1.f, 1.f, 1.f};

    float mouse_pos_x_ = 0;
    float mouse_pos_y_ = 0;
};

}  // namespace MR