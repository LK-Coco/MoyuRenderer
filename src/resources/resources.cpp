#include "resources.h"

namespace MR {

std::map<unsigned int, Texture> Resources::textures_ = std::map<unsigned int, Texture>();

Texture* Resources::load_texture(std::string name, std::string path, GLenum target, GLenum format,
                                 bool srgb) {}

}  // namespace MR