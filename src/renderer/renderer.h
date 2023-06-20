#pragma once

#include "frame_buffer.h"
#include "pipeline_states.h"
#include "vertex.h"
#include "shader_program.h"
#include "uniform.h"

namespace MR {

enum RendererType {
    Renderer_SOFT,
    Renderer_OPENGL,
    Renderer_Vulkan,
};

class Renderer {
public:
    virtual RendererType type() const = 0;
    virtual bool create() { return true; }
    virtual void destroy() {}

    // framebuffer
    virtual Ref<FrameBuffer> create_frame_buffer(bool off_screen) = 0;

    // texture
    virtual Ref<Texture> create_texture(const Texture::Desc& desc) = 0;

    // vertex
    virtual Ref<VertexArrayObject> create_vertex_array_object(const VertexArray& vertexArray) = 0;

    // shader program
    virtual Ref<ShaderProgram> create_shader_program() = 0;

    // pipeline states
    virtual Ref<PipelineStates> create_pipeline_states(const RenderStates& renderStates) = 0;

    // uniform
    virtual Ref<UniformBlock> create_uniform_block(const std::string& name, int size) = 0;
    virtual Ref<UniformSampler> create_uniform_sampler(const std::string& name,
                                                       const Texture::Desc& desc) = 0;

    // pipeline
    virtual void begin_render_pass(Ref<FrameBuffer>& frame_buffer,
                                   const ClearStates& clear_states) = 0;
    virtual void set_viewport(int x, int y, int width, int height) = 0;
    virtual void set_vertex_array_object(Ref<VertexArrayObject>& vao) = 0;
    virtual void set_shader_program(Ref<ShaderProgram>& shader_program) = 0;
    virtual void set_shader_resource(Ref<ShaderResources>& shader_resources) = 0;
    virtual void set_pipeline_states(Ref<PipelineStates>& pipeline_states) = 0;
    virtual void draw() = 0;
    virtual void end_render_pass() = 0;
    virtual void wait_idle() = 0;
};

}  // namespace MR