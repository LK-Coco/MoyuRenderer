#pragma once

#include <map>
#include "texture.h"

namespace MR {

class Resources {
public:
    static Texture* load_texture(std::string name, std::string path, GLenum target = GL_TEXTURE_2D,
                                 GLenum format = GL_RGBA, bool srgb = false);

private:
    Resources();

    static std::map<unsigned int, Texture> textures_;
};

}  // namespace MR