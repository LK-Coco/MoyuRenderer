#include "renderer_soft.h"

namespace MR {

static bool inside_triangle(int x, int y, const glm::vec4* _v) {
    glm::vec3 v[3];
    for (int i = 0; i < 3; i++) v[i] = {_v[i].x, _v[i].y, 1.0};
    glm::vec3 f0, f1, f2;
    f0 = glm::cross(v[1], v[0]);
    f1 = glm::cross(v[2], v[0]);
    f2 = glm::cross(v[0], v[2]);

    glm::vec3 p(x, y, 1.);

    if ((glm::dot(p, f0) * glm::dot(f0, v[2]) > 0) && (glm::dot(p, f1) * glm::dot(f1, v[0]) > 0) &&
        (glm::dot(p, f2) * glm::dot(f2, v[1]) > 0))
        return true;
    return false;
}

static std::tuple<float, float, float> compute_barycentric2d(float x, float y, const glm::vec4* v) {
    float c1 = (x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * y + v[1].x * v[2].y - v[2].x * v[1].y) /
               (v[0].x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * v[0].y + v[1].x * v[2].y -
                v[2].x * v[1].y);
    float c2 = (x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * y + v[2].x * v[0].y - v[0].x * v[2].y) /
               (v[1].x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * v[1].y + v[2].x * v[0].y -
                v[0].x * v[2].y);
    float c3 = (x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * y + v[0].x * v[1].y - v[1].x * v[0].y) /
               (v[2].x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * v[2].y + v[0].x * v[1].y -
                v[1].x * v[0].y);
    return {c1, c2, c3};
}

void RendererSoft::set_vertex_array_object(Ref<VertexArrayObject>& vao) {
    vao_ = static_cast<VertexArrayObjectSoft*>(vao.get());
}

void RendererSoft::set_viewport(int x, int y, int width, int height) {
    viewport_.x = (float)x;
    viewport_.y = (float)y;
    viewport_.width = (float)width;
    viewport_.height = (float)height;

    viewport_.min_depth = 0.f;
    viewport_.max_depth = 1.f;

    viewport_.abs_min_depth = std::min(viewport_.min_depth, viewport_.max_depth);
    viewport_.abs_max_depth = std::max(viewport_.min_depth, viewport_.max_depth);

    viewport_.inner_o.x = viewport_.x + viewport_.width / 2.f;
    viewport_.inner_o.y = viewport_.y + viewport_.height / 2.f;
    viewport_.inner_o.z = viewport_.min_depth;
    viewport_.inner_o.w = 0.f;

    viewport_.inner_p.x = viewport_.width / 2.f;   // divide by 2 in advance
    viewport_.inner_p.y = viewport_.height / 2.f;  // divide by 2 in advance
    viewport_.inner_p.z = viewport_.max_depth - viewport_.min_depth;
    viewport_.inner_p.w = 1.f;
}

void RendererSoft::set_shader_program(Ref<ShaderProgram>& shader_program) {}

void RendererSoft::set_shader_resource(Ref<ShaderResources>& shader_resources) {}

void RendererSoft::set_pipeline_states(Ref<PipelineStates>& pipeline_states) {}

void RendererSoft::draw() {
    process_primitive();
    process_vertex_shader();
    process_rasterization();
}

void RendererSoft::end_render_pass() {}

void RendererSoft::wait_idle() {}

void RendererSoft::process_vertex_shader() {
    uint8_t* vertexPtr = vao_->vertexes.data();
    vertexes_.resize(vao_->vertex_count);
    for (int idx = 0; idx < vao_->vertex_count; idx++) {
        VertexHolder& holder = vertexes_[idx];
        holder.discard = false;
        holder.index = idx;
        holder.vertex = vertexPtr;
        // holder.varyings =
        //     (varyingsAlignedSize_ > 0) ? (varyingBuffer + idx * varyingsAlignedCnt_) : nullptr;
        // vertexShaderImpl(holder);
        vertexPtr += vao_->vertex_stride;
    }
}

void RendererSoft::process_primitive() {
    switch (primitive_type_) {
        case Primitive_POINT: break;
        case Primitive_LINE: break;
        case Primitive_TRIANGLE: process_fill_triangles(); break;
    }
}

void RendererSoft::process_fill_triangles() {
    primitives_.resize(vao_->indices_count / 3);
    for (int idx = 0; idx < primitives_.size(); idx++) {
        auto& triangle = primitives_[idx];
        triangle.indices[0] = vao_->indices[idx * 3];
        triangle.indices[1] = vao_->indices[idx * 3 + 1];
        triangle.indices[2] = vao_->indices[idx * 3 + 2];
        triangle.discard = false;
    }
}

void RendererSoft::process_rasterization() {
    switch (primitive_type_) {
        case Primitive_POINT: break;
        case Primitive_LINE: break;
        case Primitive_TRIANGLE: process_rasterize_triangles(); break;
    }
}

void RendererSoft::process_rasterize_triangles() {
    for (auto& triangle : primitives_) {
        if (triangle.discard) continue;

        rasterize_triangles(&vertexes_[triangle.indices[0]], &vertexes_[triangle.indices[1]],
                            &vertexes_[triangle.indices[2]], triangle.front_facing);
    }
}

void RendererSoft::rasterize_triangles(VertexHolder* v0, VertexHolder* v1, VertexHolder* v2,
                                       bool front_acing) {
    glm::vec4 v[3] = {v0->frag_pos, v1->frag_pos, v2->frag_pos};
    int bboxmin_x = std::floor(std::min(v[0][0], std::min(v[1][0], v[2][0])));
    int bboxmin_y = std::floor(std::min(v[0][1], std::min(v[1][1], v[2][1])));
    int bboxmax_x = std::ceil(std::max(v[0][0], std::max(v[1][0], v[2][0])));
    int bboxmax_y = std::ceil(std::max(v[0][1], std::max(v[1][1], v[2][1])));

    for (int x = bboxmin_x; x <= bboxmax_x; ++x) {
        for (int y = bboxmin_y; y <= bboxmax_y; ++y) {
            if (inside_triangle(x + .5f, y + .5f, v)) {
                auto [px, py, pz] = compute_barycentric2d(x, y, v);
                float w = 1.f / (px / v[0].w + py / v[1].w + pz / v[2].w);
                float z_interpolated =
                    px * v[0][2] / v[0][3] + py * v[1][2] / v[1][3] + pz * v[2][2] / v[2][3];
                z_interpolated *= w;
            }
        }
    }
}

}  // namespace MR