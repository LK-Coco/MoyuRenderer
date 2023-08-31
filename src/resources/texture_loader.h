#pragma once

#include "texture.h"

namespace MR {

class TextureLoader {
public:
    static Texture load_texture(const std::string& path, GLenum target, GLenum internal_format,
                                bool srgb = false);
};

}  // namespace MR