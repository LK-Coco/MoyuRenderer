#pragma once

#include <map>
#include <memory>
#include "shading/shader.h"
#include "shading/texture.h"
#include "shading/texture_cube.h"

namespace MR {

class Resources {
public:
    static Texture* load_texture(const std::string& name,
                                 const std::string& path,
                                 GLenum target = GL_TEXTURE_2D,
                                 GLenum format = GL_RGBA, bool srgb = false);

    static Texture* load_hdr_texture(const std::string& name,
                                     const std::string& path);

    static TextureCube* load_texture_cube(const std::string& name,
                                          const std::string& folder);

    static TextureCube* environment_map_to_cubemap(
        const std::string& name, Texture* tex, std::shared_ptr<Shader>& shader);

    static Texture* get_texture(const std::string& name);

    static TextureCube* get_texture_cube(const std::string& name);

    static bool update_texture(const std::string& name, const std::string& path,
                               GLenum target = GL_TEXTURE_2D,
                               GLenum format = GL_RGBA, bool srgb = false);

    static bool update_texture(Texture* tex, const std::string& path,
                               GLenum target = GL_TEXTURE_2D,
                               GLenum format = GL_RGBA, bool srgb = false);

    static bool update_hdr_texture(const std::string& name,
                                   const std::string& path);

    static bool update_hdr_texture(Texture* tex, const std::string& path);

    static bool update_texture_cube(const std::string& name,
                                    const std::string& folder);

    static bool update_texture_cube(TextureCube* tex_cube,
                                    const std::string& folder);

    static void release(const std::string& name);

    static void release(Texture* tex);

    static void release(TextureCube* tex_cube);

    static void release_all();

private:
    Resources();

    static std::map<unsigned int, Texture> textures_;
    static std::map<unsigned int, TextureCube> textures_cube_;
};

}  // namespace MR