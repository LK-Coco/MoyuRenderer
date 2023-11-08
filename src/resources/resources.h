#pragma once

#include <map>
#include "shading/texture.h"
#include "shading/texture_cube.h"

namespace MR {

class Resources {
public:
    static Texture* load_texture(const std::string& name,
                                 const std::string& path,
                                 GLenum target = GL_TEXTURE_2D,
                                 GLenum format = GL_RGBA, bool srgb = false);

    static TextureCube* load_texture_cube(const std::string& name,
                                          const std::string& folder);

    static Texture* get_texture(const std::string& name);

    static TextureCube* get_texture_cube(const std::string& name);

    static void release(const std::string& name);

    static void release(Texture* tex);

    static void release(TextureCube* tex_cube);

    static bool update_texture(const std::string& name, const std::string& path,
                               GLenum target = GL_TEXTURE_2D,
                               GLenum format = GL_RGBA, bool srgb = false);

    static bool update_texture(Texture* tex, const std::string& path,
                               GLenum target = GL_TEXTURE_2D,
                               GLenum format = GL_RGBA, bool srgb = false);

    static bool update_texture_cube(const std::string& name,
                                    const std::string& folder);

    static bool update_texture_cube(TextureCube* tex_cube,
                                    const std::string& folder);

private:
    Resources();

    static std::map<unsigned int, Texture> textures_;
    static std::map<unsigned int, TextureCube> textures_cube_;
};

}  // namespace MR