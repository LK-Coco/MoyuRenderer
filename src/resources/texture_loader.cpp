#include <vector>
#include <iostream>
#include "texture_loader.h"
#include "stb_image.h"

namespace MR {

Texture TextureLoader::load_texture(const std::string &path, GLenum target,
                                    GLenum internal_format, bool srgb) {
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

    int width, height, nr_components;
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
            texture.generate(width, texture.internal_format, format,
                             GL_UNSIGNED_BYTE, data);
        else if (target == GL_TEXTURE_2D)
            texture.generate(width, height, texture.internal_format, format,
                             GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
        return texture;
    }
    texture.width = width;
    texture.height = height;

    return texture;
}

TextureCube TextureLoader::load_texture_cube(
    std::string top, std::string bottom, std::string left, std::string right,
    std::string front, std::string back) {
    TextureCube texture;

    stbi_set_flip_vertically_on_load(false);

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

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
            return texture;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // if (texture.mipmapping) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    texture.id = texture_id;

    return texture;
}
TextureCube TextureLoader::load_texture_cube(std::string path) {
    return TextureLoader::load_texture_cube(
        path + "right.jpg", path + "left.jpg", path + "top.jpg",
        path + "bottom.jpg", path + "front.jpg", path + "back.jpg");
    // TextureCube texture;

    // int width, height, nr_components;
    // unsigned char *data =
    //     stbi_load(path.c_str(), &width, &height, &nr_components, 0);
    // if (data) {
    //     GLenum format;
    //     if (nr_components == 3)
    //         format = GL_RGB;
    //     else
    //         format = GL_RGBA;

    //     for (unsigned int i = 0; i < 6; ++i)
    //         texture.generate_face(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width,
    //                               height, format, GL_UNSIGNED_BYTE, data);

    //     stbi_image_free(data);
    // } else {
    //     stbi_image_free(data);
    //     return texture;
    // }

    // if (texture.mipmapping) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // return texture;
}

}  // namespace MR