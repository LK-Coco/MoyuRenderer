#include "texture_cache.h"
#include <memory>
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cube_capture.h"
#include "object/cube.h"

namespace MR {

std::unordered_map<unsigned int, std::shared_ptr<Texture>>
    TextureCache::tex_cache_{};

std::shared_ptr<Texture> TextureCache::load(const std::string& name,
                                            const std::filesystem::path& path,
                                            GLenum target) {
    auto id = SHASH(name);

    if (auto item = tex_cache_.find(id); item != tex_cache_.end()) {
        return item->second;
    }

    // TODO check path valid

    auto p = std::filesystem::absolute(path);

    stbi_set_flip_vertically_on_load(true);

    auto f = stbi__fopen(p.string().c_str(), "rb");
    assert(f);
    const auto hdr = stbi_is_hdr_from_file(f);

    int width, height, nr_components;
    auto data =
        hdr ? (void*)stbi_loadf_from_file(f, &width, &height, &nr_components, 0)
            : (void*)stbi_load_from_file(f, &width, &height, &nr_components, 0);
    fclose(f);
    assert(data);

    std::shared_ptr<Texture> tex = std::make_shared<Texture>(target);
    tex->is_hdr = hdr;
    uint32_t num_levels{1u};
    if (Utils::is_power_of2(width) && Utils::is_power_of2(height))
        num_levels = Utils::calc_mip_levels(glm::max(width, height));
    GLenum internal_format;
    GLenum format;
    if (nr_components == 1) {
        format = GL_RED;
        internal_format = GL_R8;
    } else if (nr_components == 3) {
        format = GL_RGB;
        internal_format = hdr ? GL_RGB16F : GL_RGB8;
    } else if (nr_components == 4) {
        format = GL_RGBA;
        internal_format = hdr ? GL_RGBA16F : GL_RGBA8;
    }

    if (target == GL_TEXTURE_1D) {
        tex->set_storage_1d(num_levels, internal_format, width);
        tex->set_sub_image_1d(num_levels, 0, width, format, GL_UNSIGNED_BYTE,
                              data);
    } else if (target == GL_TEXTURE_2D) {
        tex->set_storage_2d(num_levels, internal_format, width, height);
        tex->set_sub_image_2d(num_levels, 0, 0, width, height, format,
                              GL_UNSIGNED_BYTE, data);
    } else if (target == GL_TEXTURE_CUBE_MAP) {
        tex->set_storage_2d(num_levels, internal_format, width, height);
        tex->set_sub_image_3d(num_levels, 0, 0, 6, width, height, 1, format,
                              GL_FLOAT, data);
    }
    const auto is_mipmap = num_levels > 1;
    tex->set(GL_TEXTURE_MIN_FILTER,
             is_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    tex->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (is_mipmap) tex->generate_mipmap();

    stbi_image_free(data);

    tex_cache_[id] = tex;

    return tex_cache_[id];
}

std::shared_ptr<Texture> TextureCache::hdri_to_env_cubemap(
    const std::string& name, std::shared_ptr<Shader>& hdri,
    std::shared_ptr<Shader>& shader, FrameBuffer& fbo) {
    auto id = SHASH(name);

    std::shared_ptr<Texture> env_cubemap =
        std::make_shared<Texture>(GL_TEXTURE_CUBE_MAP);
    auto num_levels = Utils::calc_mip_levels(512);
    env_cubemap->set_storage_2d(num_levels, GL_RGB16F, 512, 512);
    env_cubemap->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    env_cubemap->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    shader->use();
    shader->set_int("equirectangularMap", 0);
    shader->set_mat4("projection", capture_projection);

    glViewport(0, 0, 512, 512);
    fbo.bind();
    Cube cube;

    // for (unsigned int i = 0; i < 6; ++i) {
    //     shader->set_mat4("view", capture_views[i]);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
    //                            cube_tex.id, 0);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     cube.render();  // renders a 1x1 cube
    // }

    return env_cubemap;
}

std::shared_ptr<Texture> TextureCache::env_cubemap_to_irradiance_map(
    const std::string& name, std::shared_ptr<Texture>& env_cubemap,
    std::shared_ptr<Shader>& shader, FrameBuffer& fbo, RenderBuffer& rbo) {
    auto size = 32;

    auto id = SHASH(name);
    rbo.set_storage(GL_DEPTH_COMPONENT24, size, size);

    auto num_levels = Utils::calc_mip_levels(size);

    Texture irr_map{GL_TEXTURE_CUBE_MAP};
    irr_map.set_storage_2d(0, GL_RGB16F, size, size);
    irr_map.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    irr_map.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    irr_map.set(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    irr_map.set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    irr_map.set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    shader->use();
    shader->set_int("equirectangularMap", 0);
    shader->set_mat4("projection", capture_projection);

    env_cubemap->bind(0);

    glViewport(0, 0, size, size);
    fbo.bind();
    Cube cube;

    const auto& clear_value = glm::vec4(0.0f);

    for (unsigned int i = 0; i < 6; ++i) {
        shader->set_mat4("view", capture_views[i]);
        irr_map.create_face_view(0, 0, i);
        fbo.attach_texture(GL_COLOR_ATTACHMENT0 + i, irr_map);
        fbo.clear(GL_COLOR, i, glm::value_ptr(clear_value));

        cube.render();
    }
    fbo.unbind();

    auto ptr_irr_map = std::make_shared<Texture>(std::move(irr_map));
    tex_cache_[id] = ptr_irr_map;
    return ptr_irr_map;
}

std::shared_ptr<Texture> TextureCache::env_cubemap_to_prefilter_map(
    const std::string& name, std::shared_ptr<Shader>& env_cubemap,
    std::shared_ptr<Shader>& shader, FrameBuffer& fbo, RenderBuffer& rbo) {}

std::shared_ptr<Texture> TextureCache::clac_brdf_lut(
    const std::string& name, std::shared_ptr<Shader>& shader, FrameBuffer& fbo,
    RenderBuffer& rbo) {}

}  // namespace MR