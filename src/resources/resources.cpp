#include "resources.h"
#include <cstddef>
#include "utility.h"
#include "texture_loader.h"
#include "frame_buffer.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "cube.h"

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
    const std::string& name, Texture* tex, std::shared_ptr<Shader>& shader) {
    unsigned int id = SHASH(name);

    auto fbo = FrameBuffer(512, 512, false);
    TextureCube cube_tex;
    cube_tex.generate_texture_id();
    cube_tex.generate_empty_face(512, 512, GL_RGB16F, GL_RGB, GL_FLOAT,
                                 nullptr);

    glm::mat4 captureProjection =
        glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
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
    shader->set_mat4("projection", captureProjection);

    tex->bind(0);
    glViewport(0, 0, 512, 512);
    fbo.bind();
    Cube cube;
    std::shared_ptr<Material> mat = nullptr;
    for (unsigned int i = 0; i < 6; ++i) {
        shader->set_mat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cube_tex.id,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.render(mat);  // renders a 1x1 cube
    }
    fbo.unbind();

    textures_cube_[id] = cube_tex;

    return &textures_cube_[id];
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