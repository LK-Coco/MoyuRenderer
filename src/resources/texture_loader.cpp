#include <vector>
#include "texture_loader.h"
#include "stb_image.h"

namespace MR {

Texture TextureLoader::load_texture(const std::string &path, GLenum target, GLenum internal_format,
                                    bool srgb) {
    Texture texture;
    texture.target = target;
    texture.internal_format = internal_format;
    if (texture.internal_format == GL_RGB || texture.internal_format == GL_SRGB)
        texture.internal_format = srgb ? GL_SRGB : GL_RGB;
    if (texture.internal_format == GL_RGBA || texture.internal_format == GL_SRGB_ALPHA)
        texture.internal_format = srgb ? GL_SRGB_ALPHA : GL_RGBA;

    // flip textures on their y coordinate while loading
    stbi_set_flip_vertically_on_load(true);

    int width, height, nr_components;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);
    if (data) {
        GLenum format;
        if (nr_components == 1)
            format = GL_RED;
        else if (nr_components == 3)
            format = GL_RGB;
        else if (nr_components == 4)
            format = GL_RGBA;

        if (target == GL_TEXTURE_1D)
            texture.generate(width, texture.internal_format, format, GL_UNSIGNED_BYTE, data);
        else if (target == GL_TEXTURE_2D)
            texture.generate(width, height, texture.internal_format, format, GL_UNSIGNED_BYTE,
                             data);
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
        return texture;
    }
    texture.width = width;
    texture.height = height;

    return texture;
}

TextureCube TextureLoader::load_texture_cube(std::string top, std::string bottom, std::string left,
                                             std::string right, std::string front,
                                             std::string back) {
    TextureCube texture;

    stbi_set_flip_vertically_on_load(false);

    std::vector<std::string> faces = {top, bottom, left, right, front, back};
    for (unsigned int i = 0; i < faces.size(); ++i) {
        int width, height, nr_components;
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nr_components, 0);

        if (data) {
            GLenum format;
            if (nr_components == 3)
                format = GL_RGB;
            else
                format = GL_RGBA;

            texture.generate_face(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width, height, format,
                                  GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            stbi_image_free(data);
            return texture;
        }
    }
    if (texture.mipmapping) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return texture;
}
TextureCube TextureLoader::load_texture_cube(std::string folder) {
    return TextureLoader::load_texture_cube(folder + "right.jpg", folder + "left.jpg",
                                            folder + "top.jpg", folder + "bottom.jpg",
                                            folder + "front.jpg", folder + "back.jpg");
}

}  // namespace MR