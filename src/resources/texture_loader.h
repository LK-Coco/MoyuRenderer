#pragma once

#include <optional>
#include "shading/texture.h"
#include "shading/texture_cube.h"

namespace MR {

class TextureLoader {
public:
    static std::optional<Texture> load_texture(const std::string& path,
                                               GLenum target,
                                               GLenum internal_format,
                                               bool srgb = false);

    static std::optional<Texture> load_hdr_texture(const std::string& path);

    static std::optional<TextureCube> load_texture_cube(
        std::string top, std::string bottom, std::string left,
        std::string right, std::string front, std::string back);

    static std::optional<TextureCube> load_texture_cube(std::string path);

    static bool update_texture(Texture& tex, const std::string& path,
                               GLenum target, GLenum internal_format,
                               bool srgb = false);

    static bool update_hdr_texture(Texture& tex, const std::string& path);

    static bool update_texture_cube(TextureCube& tex_cube, std::string path);

    static bool update_texture_cube(TextureCube& tex_cube, std::string top,
                                    std::string bottom, std::string left,
                                    std::string right, std::string front,
                                    std::string back);
};

}  // namespace MR