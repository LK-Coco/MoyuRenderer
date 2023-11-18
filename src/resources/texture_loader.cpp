#include <optional>
#include <vector>
#include <iostream>
#include "texture_loader.h"
#include "stb_image.h"

namespace MR {

std::optional<Texture> TextureLoader::load_texture(const std::string &path,
                                                   GLenum target,
                                                   GLenum internal_format,
                                                   bool srgb) {
    Texture texture;
    texture.target = target;
    texture.internal_format = internal_format;
    if (texture.internal_format == GL_RGB || texture.internal_format == GL_SRGB)
        texture.internal_format = srgb ? GL_SRGB : GL_RGB;
    if (texture.internal_format == GL_RGBA ||
        texture.internal_format == GL_SRGB_ALPHA)
        texture.internal_format = srgb ? GL_SRGB_ALPHA : GL_RGBA;

    // flip textures on their y coordinate while loading
    // stbi_set_flip_vertically_on_load(true);
    if (update_texture(texture, path, texture.target, texture.internal_format,
                       srgb))
        return texture;
    else
        return std::nullopt;
}

std::optional<Texture> TextureLoader::load_hdr_texture(
    const std::string &path) {
    Texture texture;
    texture.target = GL_TEXTURE_2D;
    texture.internal_format = GL_RGB16F;
    texture.is_hdr = true;

    if (update_hdr_texture(texture, path))
        return texture;
    else
        return std::nullopt;
}

bool TextureLoader::update_texture(Texture &tex, const std::string &path,
                                   GLenum target, GLenum internal_format,
                                   bool srgb) {
    int width, height, nr_components;
    // stbi_set_flip_vertically_on_load(true);
    unsigned char *data =
        stbi_load(path.c_str(), &width, &height, &nr_components, 0);
    if (data) {
        GLenum format;
        if (nr_components == 1)
            format = GL_RED;
        else if (nr_components == 3)
            format = GL_RGB;
        else if (nr_components == 4)
            format = GL_RGBA;

        if (target == GL_TEXTURE_1D)
            tex.generate_1d(width, tex.internal_format, format,
                            GL_UNSIGNED_BYTE, data);
        else if (target == GL_TEXTURE_2D)
            tex.generate_2d(width, height, tex.internal_format, format,
                            GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
        return false;
    }
    tex.width = width;
    tex.height = height;

    return true;
}

bool TextureLoader::update_hdr_texture(Texture &tex, const std::string &path) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        tex.generate_2d(width, height, GL_RGB16F, GL_RGB, GL_FLOAT, data);
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
        return false;
    }
    tex.width = width;
    tex.height = height;

    return true;
}

std::optional<TextureCube> TextureLoader::load_texture_cube(
    std::string top, std::string bottom, std::string left, std::string right,
    std::string front, std::string back) {
    TextureCube texture;

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    texture.id = texture_id;

    if (update_texture_cube(texture, top, bottom, left, right, front, back))
        return texture;
    else
        return std::nullopt;
}

std::optional<TextureCube> TextureLoader::load_texture_cube(std::string path) {
    return TextureLoader::load_texture_cube(
        path + "right.jpg", path + "left.jpg", path + "top.jpg",
        path + "bottom.jpg", path + "front.jpg", path + "back.jpg");
}

bool TextureLoader::update_texture_cube(TextureCube &tex_cube,
                                        std::string path) {
    return update_texture_cube(tex_cube, path + "right.jpg", path + "left.jpg",
                               path + "top.jpg", path + "bottom.jpg",
                               path + "front.jpg", path + "back.jpg");
}

bool TextureLoader::update_texture_cube(TextureCube &tex_cube, std::string top,
                                        std::string bottom, std::string left,
                                        std::string right, std::string front,
                                        std::string back) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube.id);

    stbi_set_flip_vertically_on_load(false);
    std::vector<std::string> faces = {top, bottom, left, right, front, back};
    for (unsigned int i = 0; i < faces.size(); ++i) {
        int width, height, nr_components;
        unsigned char *data =
            stbi_load(faces[i].c_str(), &width, &height, &nr_components, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            stbi_image_free(data);
            std::cout << "load fail:" << faces[i] << std::endl;
            return false;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

}  // namespace MR