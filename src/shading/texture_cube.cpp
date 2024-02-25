#include "texture_cube.h"

namespace MR {

TextureCube::TextureCube() {}
TextureCube::~TextureCube() {}

void TextureCube::generate_texture_id() { glGenTextures(1, &id); }

void TextureCube::generate_with_type(const int width, const int height,
                                     CubeMapType type) {
    switch (type) {
        case CubeMapType::SHADOW_MAP:
            for (unsigned int i = 0; i < NUM_FACE; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                             GL_DEPTH_COMPONENT, width, height, 0,
                             GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                            GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                            GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                            GL_CLAMP_TO_EDGE);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE,
            //                 GL_COMPARE_REF_TO_TEXTURE);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
            //                 GL_LINEAR);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
            //                 GL_LINEAR);
            break;

        case CubeMapType::HDR_MAP:
            for (unsigned int i = 0; i < NUM_FACE; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
                             width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                            GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                            GL_CLAMP_TO_EDGE);
            break;

        case CubeMapType::PREFILTER_MAP:
            for (unsigned int i = 0; i < NUM_FACE; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                             width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                            GL_LINEAR);

            // For the specular IBL component we use the mipmap levels to store
            // increasingly rougher representations of the environment. And then
            // interpolater between those
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            max_mip_levels = 5;
            break;
    }
}

void TextureCube::generate_face(GLenum face, unsigned int width,
                                unsigned int height, GLenum format, GLenum type,
                                unsigned char* data) {
    face_width = width;
    face_height = height;
    this->format = format;
    this->type = type;

    glTexImage2D(face, 0, format, width, height, 0, format, type, data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_r);
}

void TextureCube::generate_empty_face(unsigned int width, unsigned int height,
                                      GLenum internal_format, GLenum format,
                                      GLenum type, unsigned char* data) {
    face_width = width;
    face_height = height;
    this->internal_format = internal_format;
    this->format = format;
    this->type = type;

    // bind();
    for (unsigned int i = 0; i < 6; ++i) {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format,
                     width, height, 0, format, type, data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_r);
}

void TextureCube::set_mip_face(GLenum face, unsigned int width,
                               unsigned int height, GLenum format, GLenum type,
                               unsigned int mip_level, unsigned char* data) {
    // bind();
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip_level, 0, 0,
                    width, height, format, type, data);
}

void TextureCube::resize(unsigned int width, unsigned int height) {
    face_width = width;
    face_height = height;

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width,
                     height, 0, format, type, nullptr);
}

void TextureCube::gen_mipmap() { glGenerateMipmap(GL_TEXTURE_CUBE_MAP); }

void TextureCube::bind(int unit) {
    if (unit >= 0) glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void TextureCube::unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

void TextureCube::release() { glDeleteTextures(1, &id); }

}  // namespace MR