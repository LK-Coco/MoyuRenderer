#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "shading/texture.h"
#include "shading/shader.h"
#include "frame_buffer.h"
#include "render_buffer.h"

namespace MR {

class TextureCache {
public:
    static std::shared_ptr<Texture> load(const std::string& name,
                                         const std::filesystem::path& path,
                                         GLenum target);

    static std::shared_ptr<Texture> hdri_to_env_cubemap(
        const std::string& name, std::shared_ptr<Shader>& hdri,
        std::shared_ptr<Shader>& shader, FrameBuffer& fbo);

    static std::shared_ptr<Texture> env_cubemap_to_irradiance_map(
        const std::string& name, std::shared_ptr<Texture>& env_cubemap,
        std::shared_ptr<Shader>& shader, FrameBuffer& fbo, RenderBuffer& rbo);

    static std::shared_ptr<Texture> env_cubemap_to_prefilter_map(
        const std::string& name, std::shared_ptr<Shader>& env_cubemap,
        std::shared_ptr<Shader>& shader, FrameBuffer& fbo, RenderBuffer& rbo);

    static std::shared_ptr<Texture> clac_brdf_lut(
        const std::string& name, std::shared_ptr<Shader>& shader,
        FrameBuffer& fbo, RenderBuffer& rbo);

private:
    TextureCache();

    static std::unordered_map<unsigned int, std::shared_ptr<Texture>>
        tex_cache_;
};

}  // namespace MR