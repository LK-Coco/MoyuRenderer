#pragma once

#include <string>
#include "glad/glad.h"

namespace MR {

class Texture {
public:
    GLenum internal_format = GL_RGBA;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    GLenum filter_min = GL_LINEAR_MIPMAP_LINEAR;
    GLenum filter_max = GL_LINEAR;
    GLenum wrap_s = GL_REPEAT;
    GLenum wrap_t = GL_REPEAT;
    GLenum wrap_r = GL_REPEAT;
    bool mipmapping = true;
    bool is_hdr = false;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int depth = 0;

    Texture(GLenum target);
    ~Texture();

    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

    Texture(Texture&& other) = default;

    inline unsigned int get_id() const { return id_; }

    void set_buffer(GLenum internal_format, GLuint buffer);

    void set_buffer_range(GLenum internal_format, GLuint buffer,
                          GLintptr offset, GLsizeiptr size);

    void set_storage_1d(GLsizei levels, GLenum internal_format, GLsizei width);

    void set_storage_2d(GLsizei levels, GLenum internal_format, GLsizei width,
                        GLsizei height);

    void set_storage_3d(GLsizei levels, GLenum internal_format, GLsizei width,
                        GLsizei height, GLsizei depth);

    void set_storage_2d_multisample(GLsizei samples, GLenum internal_format,
                                    GLsizei width, GLsizei height,
                                    GLboolean fixed_samplelocations);

    void set_storage_3d_multisample(GLsizei samples, GLenum internal_format,
                                    GLsizei width, GLsizei height,
                                    GLsizei depth,
                                    GLboolean fixed_samplelocations);

    void set_sub_image_1d(GLint level, GLint xoffset, GLsizei width,
                          GLenum format, GLenum type, const void* pixels);

    void set_sub_image_2d(GLint level, GLint xoffset, GLint yoffset,
                          GLsizei width, GLsizei height, GLenum format,
                          GLenum type, const void* pixels);

    void set_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                          GLint zoffset, GLsizei width, GLsizei height,
                          GLsizei depth, GLenum format, GLenum type,
                          const void* pixels);

    void set_compressed_sub_image_1d(GLint level, GLint xoffset, GLsizei width,
                                     GLenum format, GLsizei imageSize,
                                     const void* data);

    void set_compressed_sub_image_2d(GLint level, GLint xoffset, GLint yoffset,
                                     GLsizei width, GLsizei height,
                                     GLenum format, GLsizei imageSize,
                                     const void* data);

    void set_compressed_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                                     GLint zoffset, GLsizei width,
                                     GLsizei height, GLsizei depth,
                                     GLenum format, GLsizei imageSize,
                                     const void* data);

    void copy_sub_image_1d(GLint level, GLint xoffset, GLint x, GLint y,
                           GLsizei width);

    void copy_sub_image_2d(GLint level, GLint xoffset, GLint yoffset, GLint x,
                           GLint y, GLsizei width, GLsizei height);

    void copy_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                           GLint zoffset, GLint x, GLint y, GLsizei width,
                           GLsizei height);

    void generate_mipmap();

    void get_image(GLint level, GLenum format, GLenum type, GLsizei bufSize,
                   void* pixels);

    void get_compressed_image(GLint level, GLsizei bufSize, void* pixels);

    GLenum get_target() const;

    template <class T>
    void get(GLenum property, T* value);

    template <class T>
    T get(GLenum property);

    template <class T>
    void get(GLint level, GLenum property, T* value);

    template <class T>
    T get(GLint level, GLenum property);

    template <class T>
    void set(GLenum property, T value);

    void bind(int unit);

    void release();

private:
    unsigned int id_;
    GLenum target_;
};

template <>
inline void Texture::get<GLint>(GLenum property, GLint* value) {
    glGetTextureParameteriv(id_, property, value);
}

template <>
inline void Texture::get<GLfloat>(GLenum property, GLfloat* value) {
    glGetTextureParameterfv(id_, property, value);
}

template <>
inline GLint Texture::get<GLint>(GLenum property) {
    GLint value;
    glGetTextureParameteriv(id_, property, &value);
    return value;
}

template <>
inline GLfloat Texture::get<GLfloat>(GLenum property) {
    GLfloat value;
    glGetTextureParameterfv(id_, property, &value);
    return value;
}

/*
 * Level specific accessors definitions
 */

template <>
inline void Texture::get<GLint>(GLint level, GLenum property, GLint* value) {
    glGetTextureLevelParameteriv(id_, level, property, value);
}

template <>
inline void Texture::get<GLfloat>(GLint level, GLenum property,
                                  GLfloat* value) {
    glGetTextureLevelParameterfv(id_, level, property, value);
}

template <>
inline GLint Texture::get<GLint>(GLint level, GLenum property) {
    GLint value;
    glGetTextureLevelParameteriv(id_, level, property, &value);
    return value;
}

template <>
inline GLfloat Texture::get<GLfloat>(GLint level, GLenum property) {
    GLfloat value;
    glGetTextureLevelParameterfv(id_, level, property, &value);
    return value;
}

/*
 * Templated mutators definitions
 */

template <>
inline void Texture::set<GLint>(GLenum property, GLint value) {
    glTextureParameteri(id_, property, value);
}

template <>
inline void Texture::set<GLenum>(GLenum property, GLenum value) {
    glTextureParameteri(id_, property, static_cast<GLint>(value));
}

template <>
inline void Texture::set<GLfloat>(GLenum property, GLfloat value) {
    glTextureParameterf(id_, property, value);
}

template <>
inline void Texture::set<GLint*>(GLenum property, GLint* value) {
    glTextureParameteriv(id_, property, value);
}

template <>
inline void Texture::set<GLfloat*>(GLenum property, GLfloat* value) {
    glTextureParameterfv(id_, property, value);
}

template <>
inline void Texture::set<const GLint*>(GLenum property, const GLint* value) {
    glTextureParameteriv(id_, property, value);
}

template <>
inline void Texture::set<const GLfloat*>(GLenum property,
                                         const GLfloat* value) {
    glTextureParameterfv(id_, property, value);
}

}  // namespace MR