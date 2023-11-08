#include "resources.h"
#include <cstddef>
#include "utility.h"
#include "texture_loader.h"

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

}  // namespace MR