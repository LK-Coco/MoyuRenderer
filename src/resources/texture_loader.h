#pragma once

#include "shading/texture.h"
#include "shading/texture_cube.h"

namespace MR {

class TextureLoader {
public:
    static Texture load_texture(const std::string& path, GLenum target, GLenum internal_format,
                                bool srgb = false);
    static TextureCube load_texture_cube(std::string top, std::string bottom, std::string left,
                                         std::string right, std::string front, std::string back);
    static TextureCube load_texture_cube(std::string folder);
};

}  // namespace MR