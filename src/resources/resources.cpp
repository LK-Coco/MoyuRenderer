#include "resources.h"
#include "utility.h"
#include "texture_loader.h"

namespace MR {

std::map<unsigned int, Texture> Resources::textures_ =
    std::map<unsigned int, Texture>();
std::map<unsigned int, TextureCube> Resources::textures_cube_ =
    std::map<unsigned int, TextureCube>();

Texture* Resources::load_texture(std::string name, std::string path,
                                 GLenum target, GLenum format, bool srgb) {
    auto id = SHASH(name);
    if (textures_.find(id) != textures_.end()) {
        return &textures_[id];
    }

    auto tex = TextureLoader::load_texture(path, target, format);
    if (tex.width != 0) {
        textures_[id] = tex;
        return &textures_[id];
    }

    return nullptr;
}

TextureCube* Resources::load_texture_cube(std::string name,
                                          std::string folder) {
    unsigned int id = SHASH(name);

    if (Resources::textures_cube_.find(id) != Resources::textures_cube_.end())
        return &Resources::textures_cube_[id];

    TextureCube texture = TextureLoader::load_texture_cube(folder);
    Resources::textures_cube_[id] = texture;
    return &Resources::textures_cube_[id];
}

Texture* Resources::get_texture(std::string name) {
    unsigned int id = SHASH(name);

    if (Resources::textures_.find(id) != Resources::textures_.end()) {
        return &Resources::textures_[id];
    } else {
        return nullptr;
    }
}
TextureCube* Resources::get_texture_cube(std::string name) {
    unsigned int id = SHASH(name);

    if (Resources::textures_cube_.find(id) != Resources::textures_cube_.end()) {
        return &Resources::textures_cube_[id];
    } else {
        return nullptr;
    }
}

}  // namespace MR