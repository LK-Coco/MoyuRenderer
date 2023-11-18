#include "texture.h"

namespace MR {

void Texture::generate_1d(unsigned int w, GLenum internal_format, GLenum format,
                          GLenum type, void* data) {
    glGenTextures(1, &id);

    width = w;
    height = 0;
    depth = 0;
    this->internal_format = internal_format;
    this->format = format;
    this->type = type;

    bind();
    glTexImage1D(target, 0, internal_format, width, 0, format, type, data);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_s);
    if (mipmapping) glGenerateMipmap(target);
    unbind();
}

void Texture::generate_2d(unsigned int w, unsigned int h,
                          GLenum internal_format, GLenum format, GLenum type,
                          void* data) {
    glGenTextures(1, &id);

    width = w;
    height = h;
    depth = 0;
    this->internal_format = internal_format;
    this->format = format;
    this->type = type;

    bind();
    glTexImage2D(target, 0, internal_format, width, height, 0, format, type,
                 data);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap_t);
    if (mipmapping) glGenerateMipmap(target);
    // unbind();
}

void Texture::generate_3d(unsigned int w, unsigned int h, unsigned int d,
                          GLenum internal_format, GLenum format, GLenum type,
                          void* data) {
    glGenTextures(1, &id);

    width = w;
    height = h;
    depth = d;
    this->internal_format = internal_format;
    this->format = format;
    this->type = type;

    bind();
    glTexImage3D(target, 0, internal_format, width, height, depth, 0, format,
                 type, data);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap_r);
    if (mipmapping) glGenerateMipmap(target);
    unbind();
}

void Texture::resize(unsigned int w, unsigned int h, unsigned int d) {
    bind();
    if (target == GL_TEXTURE_1D) {
        glTexImage1D(GL_TEXTURE_1D, 0, internal_format, w, 0, format, type, 0);
    } else if (target == GL_TEXTURE_2D) {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, type,
                     0);
    } else if (target == GL_TEXTURE_3D) {
        glTexImage3D(GL_TEXTURE_3D, 0, internal_format, w, h, d, 0, format,
                     type, 0);
    }
}

void Texture::bind(int unit) {
    if (unit >= 0) glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, id);
}

void Texture::unbind() { glBindTexture(target, 0); }

void Texture::release() { glDeleteTextures(1, &id); }

void Texture::set_wrap_mode(GLenum wrap_mode, bool is_bind) {
    if (is_bind) bind();
    if (target == GL_TEXTURE_1D) {
        wrap_s = wrap_mode;
        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_mode);
    } else if (target == GL_TEXTURE_2D) {
        wrap_s = wrap_mode;
        wrap_t = wrap_mode;
        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap_mode);
    } else if (target == GL_TEXTURE_3D) {
        wrap_s = wrap_mode;
        wrap_t = wrap_mode;
        wrap_r = wrap_mode;
        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap_mode);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap_mode);
    }
}
void Texture::set_filter_min(GLenum filter, bool is_bind) {
    if (is_bind) bind();
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
}
void Texture::set_filter_max(GLenum filter, bool is_bind) {
    if (is_bind) bind();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
}

}  // namespace MR