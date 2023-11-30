#include "texture.h"

namespace MR {

Texture::Texture(GLenum target) : target_(target) {
    glCreateTextures(target, 1, &id_);
}

Texture::~Texture() { release(); }

void Texture::create_face_view(GLuint mipLevel, GLuint layer, GLuint face) {
    if (view_ != GL_NONE) glDeleteTextures(1, &view_);
    glGenTextures(1, &view_);

    glTextureView(view_, GL_TEXTURE_2D, id_,
                  static_cast<GLenum>(internal_format), mipLevel, 1,
                  (layer * 6) + face, 1);
}

void Texture::set_buffer(GLenum internal_format, GLuint buffer) {
    glTextureBuffer(id_, internal_format, buffer);
}

void Texture::set_buffer_range(GLenum internal_format, GLuint buffer,
                               GLintptr offset, GLsizeiptr size) {
    glTextureBufferRange(id_, internal_format, buffer, offset, size);
}

void Texture::set_storage_1d(GLsizei levels, GLenum internal_format,
                             GLsizei width) {
    glTextureStorage1D(id_, levels, internal_format, width);
}

void Texture::set_storage_2d(GLsizei levels, GLenum internal_format,
                             GLsizei width, GLsizei height) {
    glTextureStorage2D(id_, levels, internal_format, width, height);
}

void Texture::set_storage_3d(GLsizei levels, GLenum internal_format,
                             GLsizei width, GLsizei height, GLsizei depth) {
    glTextureStorage3D(id_, levels, internal_format, width, height, depth);
}

void Texture::set_storage_2d_multisample(GLsizei samples,
                                         GLenum internal_format, GLsizei width,
                                         GLsizei height,
                                         GLboolean fixed_samplelocations) {
    glTextureStorage2DMultisample(id_, samples, internal_format, width, height,
                                  fixed_samplelocations);
}

void Texture::set_storage_3d_multisample(GLsizei samples,
                                         GLenum internal_format, GLsizei width,
                                         GLsizei height, GLsizei depth,
                                         GLboolean fixed_samplelocations) {
    glTextureStorage3DMultisample(id_, samples, internal_format, width, height,
                                  depth, fixed_samplelocations);
}

void Texture::set_sub_image_1d(GLint level, GLint xoffset, GLsizei width,
                               GLenum format, GLenum type, const void* pixels) {
    glTextureSubImage1D(id_, level, xoffset, width, format, type, pixels);
}

void Texture::set_sub_image_2d(GLint level, GLint xoffset, GLint yoffset,
                               GLsizei width, GLsizei height, GLenum format,
                               GLenum type, const void* pixels) {
    glTextureSubImage2D(id_, level, xoffset, yoffset, width, height, format,
                        type, pixels);
}

void Texture::set_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                               GLint zoffset, GLsizei width, GLsizei height,
                               GLsizei depth, GLenum format, GLenum type,
                               const void* pixels) {
    glTextureSubImage3D(id_, level, xoffset, yoffset, zoffset, width, height,
                        depth, format, type, pixels);
}

void Texture::set_compressed_sub_image_1d(GLint level, GLint xoffset,
                                          GLsizei width, GLenum format,
                                          GLsizei imageSize, const void* data) {
    glCompressedTextureSubImage1D(id_, level, xoffset, width, format, imageSize,
                                  data);
}

void Texture::set_compressed_sub_image_2d(GLint level, GLint xoffset,
                                          GLint yoffset, GLsizei width,
                                          GLsizei height, GLenum format,
                                          GLsizei imageSize, const void* data) {
    glCompressedTextureSubImage2D(id_, level, xoffset, yoffset, width, height,
                                  format, imageSize, data);
}

void Texture::set_compressed_sub_image_3d(GLint level, GLint xoffset,
                                          GLint yoffset, GLint zoffset,
                                          GLsizei width, GLsizei height,
                                          GLsizei depth, GLenum format,
                                          GLsizei imageSize, const void* data) {
    glCompressedTextureSubImage3D(id_, level, xoffset, yoffset, zoffset, width,
                                  height, depth, format, imageSize, data);
}

void Texture::copy_sub_image_1d(GLint level, GLint xoffset, GLint x, GLint y,
                                GLsizei width) {
    glCopyTextureSubImage1D(id_, level, xoffset, x, y, width);
}

void Texture::copy_sub_image_2d(GLint level, GLint xoffset, GLint yoffset,
                                GLint x, GLint y, GLsizei width,
                                GLsizei height) {
    glCopyTextureSubImage2D(id_, level, xoffset, yoffset, x, y, width, height);
}

void Texture::copy_sub_image_3d(GLint level, GLint xoffset, GLint yoffset,
                                GLint zoffset, GLint x, GLint y, GLsizei width,
                                GLsizei height) {
    glCopyTextureSubImage3D(id_, level, xoffset, yoffset, zoffset, x, y, width,
                            height);
}

void Texture::generate_mipmap() { glGenerateTextureMipmap(id_); }

void Texture::get_image(GLint level, GLenum format, GLenum type,
                        GLsizei bufSize, void* pixels) {
    glGetTextureImage(id_, level, format, type, bufSize, pixels);
}

void Texture::get_compressed_image(GLint level, GLsizei bufSize, void* pixels) {
    glGetCompressedTextureImage(id_, level, bufSize, pixels);
}

GLenum Texture::get_target() const { return target_; }

void Texture::bind(int unit) { glBindTextureUnit(unit, id_); }

void Texture::release() {
    if (id_) glDeleteTextures(1, &id_);
}

}  // namespace MR