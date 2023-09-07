#include "texture_cube.h"

namespace MR {

TextureCube::TextureCube() {}
TextureCube::~TextureCube() {}

void TextureCube::default_initialize(unsigned int width, unsigned int height, GLenum format,
                                     GLenum type, bool mipmap) {
    glGenTextures(1, &id);

    face_width = width;
    face_height = height;
    this->format = format;
    this->type = type;
    mipmapping = mipmap;

    if (type == GL_HALF_FLOAT && format == GL_RGB)
        internal_format = GL_RGB16F;
    else if (type == GL_FLOAT && format == GL_RGB)
        internal_format = GL_RGB32F;
    else if (type == GL_HALF_FLOAT && format == GL_RGBA)
        internal_format = GL_RGBA16F;
    else if (type == GL_FLOAT && format == GL_RGBA)
        internal_format = GL_RGBA32F;

    bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_r);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0,
                     format, type, nullptr);
    }
    if (mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void TextureCube::generate_face(GLenum face, unsigned int width, unsigned int height, GLenum format,
                                GLenum type, unsigned char* data) {
    if (face_width == 0) glGenTextures(1, &id);

    face_width = width;
    face_height = height;
    this->format = format;
    this->type = type;

    bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_r);

    glTexImage2D(face, 0, format, width, height, 0, format, type, data);
}

void TextureCube::set_mip_face(GLenum face, unsigned int width, unsigned int height, GLenum format,
                               GLenum type, unsigned int mip_level, unsigned char* data) {
    bind();
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip_level, 0, 0, width, height, format,
                    type, data);
}

void TextureCube::resize(unsigned int width, unsigned int height) {
    face_width = width;
    face_height = height;

    bind();
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, type,
                     nullptr);
}

void TextureCube::bind(int unit) {
    if (unit >= 0) glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void TextureCube::unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

}  // namespace MR