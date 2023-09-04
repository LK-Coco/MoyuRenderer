#include "resources.h"
#include "utility.h"
#include "texture_loader.h"

namespace MR {

std::map<unsigned int, Texture> Resources::textures_ = std::map<unsigned int, Texture>();

Texture* Resources::load_texture(std::string name, std::string path, GLenum target, GLenum format,
                                 bool srgb) {
    auto id = SHASH(path + name);
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

}  // namespace MR