#include "resources.h"
#include <cstddef>
#include "utility.h"
#include "texture_loader.h"
#include "frame_buffer.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "object/cube.h"
#include "object/quad.h"

namespace MR {

std::map<unsigned int, Texture> Resources::textures_ =
    std::map<unsigned int, Texture>();
std::map<unsigned int, TextureCube> Resources::textures_cube_ =
    std::map<unsigned int, TextureCube>();

Texture* Resources::load_texture(const std::string& name,
                                 const std::string& path, GLenum target,
                                 GLenum format, bool srgb) {
    auto id = SHASH(name);

    if (textures_.find(id) != textures_.end()) {
        return &textures_[id];
    }

    auto tex = TextureLoader::load_texture(path, target, format);
    if (tex.has_value()) {
        textures_[id] = tex.value();
        return &textures_[id];
    }

    return nullptr;
}

Texture* Resources::load_hdr_texture(const std::string& name,
                                     const std::string& path) {
    auto id = SHASH(name);

    if (textures_.find(id) != textures_.end()) {
        return &textures_[id];
    }

    auto tex = TextureLoader::load_hdr_texture(path);
    if (tex.has_value()) {
        textures_[id] = tex.value();
        return &textures_[id];
    }

    return nullptr;
}

TextureCube* Resources::load_texture_cube(const std::string& name,
                                          const std::string& folder) {
    unsigned int id = SHASH(name);

    if (Resources::textures_cube_.find(id) != Resources::textures_cube_.end())
        return &Resources::textures_cube_[id];

    auto texture = TextureLoader::load_texture_cube(folder);
    if (texture.has_value()) {
        Resources::textures_cube_[id] = texture.value();
        return &Resources::textures_cube_[id];
    } else {
        return nullptr;
    }
}

TextureCube* Resources::environment_map_to_cubemap(
    const std::string& name, Texture* tex, std::shared_ptr<Shader>& shader,
    CaptureFBO& fbo) {
    unsigned int id = SHASH(name);

    TextureCube cube_tex;
    cube_tex.filter_min = GL_LINEAR_MIPMAP_LINEAR;
    cube_tex.generate_texture_id();
    cube_tex.bind();
    cube_tex.generate_empty_face(512, 512, GL_RGB16F, GL_RGB, GL_FLOAT,
                                 nullptr);

    glm::mat4 capture_projection =
        glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_views[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f))};

    shader->use();
    shader->set_int("equirectangularMap", 0);
    shader->set_mat4("projection", capture_projection);

    tex->bind(0);
    glViewport(0, 0, 512, 512);
    fbo.bind();
    Cube cube;

    for (unsigned int i = 0; i < 6; ++i) {
        shader->set_mat4("view", capture_views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cube_tex.id,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.render();  // renders a 1x1 cube
    }

    cube_tex.gen_mipmap();

    fbo.unbind();

    textures_cube_[id] = cube_tex;

    return &textures_cube_[id];
}

TextureCube* Resources::env_cubemap_to_irradiance_map(
    const std::string& name, TextureCube* tex, std::shared_ptr<Shader>& shader,
    CaptureFBO& fbo) {
    unsigned int id = SHASH(name);

    fbo.resize(32, 32);

    TextureCube irradiance_map;
    irradiance_map.generate_texture_id();
    irradiance_map.bind();
    irradiance_map.generate_empty_face(32, 32, GL_RGB16F, GL_RGB, GL_FLOAT,
                                       nullptr);

    glm::mat4 capture_projection =
        glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_views[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f))};

    shader->use();
    shader->set_int("environmentMap", 0);
    shader->set_mat4("projection", capture_projection);
    tex->bind(0);

    glViewport(0, 0, 32, 32);  // don't forget to configure the viewport to
    Cube cube;

    fbo.bind();
    for (unsigned int i = 0; i < 6; ++i) {
        shader->set_mat4("view", capture_views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               irradiance_map.id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.render();
    }
    fbo.unbind();

    textures_cube_[id] = irradiance_map;

    return &textures_cube_[id];
}

TextureCube* Resources::env_cubemap_to_prefilter_map(
    const std::string& name, TextureCube* env_cubemap,
    std::shared_ptr<Shader>& shader, CaptureFBO& fbo) {
    unsigned int id = SHASH(name);

    TextureCube prefilter_map;
    prefilter_map.filter_min = GL_LINEAR_MIPMAP_LINEAR;
    prefilter_map.generate_texture_id();
    prefilter_map.bind();
    prefilter_map.generate_empty_face(128, 128, GL_RGB16F, GL_RGB, GL_FLOAT,
                                      nullptr);
    prefilter_map.gen_mipmap();

    glm::mat4 capture_projection =
        glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 capture_views[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f))};

    shader->use();
    shader->set_int("environmentMap", 0);
    shader->set_mat4("projection", capture_projection);
    env_cubemap->bind(0);

    fbo.bind();
    unsigned int maxMipLevels = 5;
    Cube cube;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, fbo.fb_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth,
                              mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shader->set_float("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i) {
            shader->set_mat4("view", capture_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   prefilter_map.id, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube.render();
        }
    }

    fbo.unbind();

    textures_cube_[id] = prefilter_map;
    return &textures_cube_[id];
}

Texture* Resources::clac_brdf_lut(const std::string& name,
                                  std::shared_ptr<Shader>& shader,
                                  CaptureFBO& fbo) {
    unsigned int id = SHASH(name);

    Texture lut_tex;
    lut_tex.mipmapping = false;
    lut_tex.filter_min = GL_LINEAR;
    lut_tex.wrap_s = GL_CLAMP_TO_EDGE;
    lut_tex.wrap_t = GL_CLAMP_TO_EDGE;
    lut_tex.generate_2d(512, 512, GL_RG16F, GL_RG, GL_FLOAT, nullptr);
    // lut_tex.bind();
    //  then re-configure capture framebuffer object and render screen-space
    //  quad with BRDF shader.
    fbo.bind();
    glBindRenderbuffer(GL_RENDERBUFFER, fbo.fb_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           lut_tex.id, 0);

    glViewport(0, 0, 512, 512);
    shader->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Quad quad;
    quad.render();

    fbo.unbind();

    textures_[id] = lut_tex;
    return &textures_[id];
}

Texture* Resources::get_texture(const std::string& name) {
    unsigned int id = SHASH(name);

    if (Resources::textures_.find(id) != Resources::textures_.end()) {
        return &Resources::textures_[id];
    } else {
        return nullptr;
    }
}
TextureCube* Resources::get_texture_cube(const std::string& name) {
    unsigned int id = SHASH(name);

    if (Resources::textures_cube_.find(id) != Resources::textures_cube_.end()) {
        return &Resources::textures_cube_[id];
    } else {
        return nullptr;
    }
}

bool Resources::update_texture(const std::string& name, const std::string& path,
                               GLenum target, GLenum format, bool srgb) {
    auto id = SHASH(name);

    if (textures_.find(id) != textures_.end()) {
        Texture& tex = textures_[id];
        return TextureLoader::update_texture(tex, path, target, format, srgb);
    } else {
        return false;
    }
}

bool Resources::update_texture(Texture* tex, const std::string& path,
                               GLenum target, GLenum format, bool srgb) {
    return TextureLoader::update_texture(*tex, path, target, format, srgb);
}

bool Resources::update_hdr_texture(const std::string& name,
                                   const std::string& path) {
    auto id = SHASH(name);

    if (textures_.find(id) != textures_.end()) {
        Texture& tex = textures_[id];
        return TextureLoader::update_hdr_texture(tex, path);
    } else {
        return false;
    }
}

bool Resources::update_hdr_texture(Texture* tex, const std::string& path) {
    return TextureLoader::update_hdr_texture(*tex, path);
}

bool Resources::update_texture_cube(const std::string& name,
                                    const std::string& folder) {
    unsigned int id = SHASH(name);

    if (Resources::textures_cube_.find(id) != Resources::textures_cube_.end())
        return TextureLoader::update_texture_cube(textures_cube_[id], folder);
    else {
        return false;
    }
}

bool Resources::update_texture_cube(TextureCube* tex_cube,
                                    const std::string& folder) {
    return TextureLoader::update_texture_cube(*tex_cube, folder);
}

void Resources::release(const std::string& name) {
    auto id = SHASH(name);

    if (textures_.find(id) != textures_.end()) {
        auto tex = &textures_[id];
        tex->release();
        textures_.erase(id);
    }
}

void Resources::release(Texture* tex) {
    if (tex == nullptr) return;
    tex->release();
    for (const auto& pair : textures_) {
        if (&pair.second == tex) {
            textures_.erase(pair.first);
            break;
        }
    }
}

void Resources::release(TextureCube* tex_cube) {
    if (tex_cube == nullptr) return;
    tex_cube->release();
    for (const auto& pair : textures_cube_) {
        if (&pair.second == tex_cube) {
            textures_cube_.erase(pair.first);
            break;
        }
    }
}

void Resources::release_all() {
    for (auto& pair : textures_) {
        pair.second.release();
    }
    textures_.clear();

    for (auto& pair : textures_cube_) {
        pair.second.release();
    }
    textures_cube_.clear();
}

}  // namespace MR