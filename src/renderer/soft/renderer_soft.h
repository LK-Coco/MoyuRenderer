#pragma once

#include "core/base.h"
#include "renderer/renderer.h"
#include "vertex_soft.h"

namespace MR {

struct Viewport {
    float x;
    float y;
    float width;
    float height;
    float min_depth;
    float max_depth;

    // ref: https://registry.khronos.org/vulkan/specs/1.0/html/chap24.html#vertexpostproc-viewport
    glm::vec4 inner_o;
    glm::vec4 inner_p;

    float abs_min_depth;
    float abs_max_depth;
};

struct Primitive {
    bool discard = false;
    bool front_facing = true;
    size_t indices[3] = {0, 0, 0};
};

// 顶点着色的输出
struct VertexHolder {
    bool discard = false;
    size_t index = 0;

    void* vertex = nullptr;
    float* varyings = nullptr;

    int clip_mask = 0;
    glm::aligned_vec4 clip_pos = glm::vec4(0.f);  // clip space position
    glm::aligned_vec4 frag_pos = glm::vec4(0.f);  // screen space position

    std::shared_ptr<uint8_t> vertex_holder = nullptr;
    std::shared_ptr<float> varyings_holder = nullptr;
};

class RendererSoft : Renderer {
public:
    inline RendererType type() const override { return Renderer_SOFT; }

    // framebuffer
    Ref<FrameBuffer> create_frame_buffer(bool off_screen) override;

    // texture
    Ref<Texture> create_texture(const Texture::Desc& desc) override;

    // vertex
    Ref<VertexArrayObject> create_vertex_array_object(const VertexArray& vertexArray) override;

    // shader program
    Ref<ShaderProgram> create_shader_program() override;

    // pipeline states
    Ref<PipelineStates> create_pipeline_states(const RenderStates& renderStates) override;

    // uniform
    Ref<UniformBlock> create_uniform_block(const std::string& name, int size) override;
    Ref<UniformSampler> create_uniform_sampler(const std::string& name,
                                               const Texture::Desc& desc) override;

    void begin_render_pass(Ref<FrameBuffer>& frame_buffer,
                           const ClearStates& clear_states) override;
    void set_vertex_array_object(Ref<VertexArrayObject>& vao) override;
    void set_viewport(int x, int y, int width, int height) override;
    void set_shader_program(Ref<ShaderProgram>& shader_program) override;
    void set_shader_resource(Ref<ShaderResources>& shader_resources) override;
    void set_pipeline_states(Ref<PipelineStates>& pipeline_states) override;
    void draw() override;
    void end_render_pass() override;
    void wait_idle() override;

private:
    void process_vertex_shader();
    void process_primitive();
    void process_fill_triangles();
    void process_clipping();
    void process_face_culling();
    void process_rasterization();
    void process_rasterize_triangles();
    void rasterize_triangles(VertexHolder* v0, VertexHolder* v1, VertexHolder* v2,
                             bool front_acing);
    void draw_line();

private:
    Viewport viewport_;
    PrimitiveType primitive_type_ = Primitive_TRIANGLE;
    std::vector<Primitive> primitives_;
    std::vector<VertexHolder> vertexes_;

    std::shared_ptr<float> varyings_ = nullptr;
    size_t varyings_cnt_ = 0;
    size_t varyings_aligned_cnt_ = 0;
    size_t varyings_aligned_size_ = 0;

    VertexArrayObjectSoft* vao_ = nullptr;
};

}  // namespace MR