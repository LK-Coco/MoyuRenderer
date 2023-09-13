#pragma once

#include "glad/glad.h"

namespace MR {

class TextureCube {
public:
    unsigned int id;

    GLenum target = GL_TEXTURE_2D;
    GLenum internal_format = GL_RGBA;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    GLenum filter_min = GL_LINEAR;
    GLenum filter_max = GL_LINEAR;
    GLenum wrap_s = GL_CLAMP_TO_EDGE;
    GLenum wrap_t = GL_CLAMP_TO_EDGE;
    GLenum wrap_r = GL_CLAMP_TO_EDGE;
    bool mipmapping = true;

    unsigned int face_width = 0;
    unsigned int face_height = 0;

    TextureCube();
    ~TextureCube();

    void generate_face(GLenum face, unsigned int width, unsigned int height,
                       GLenum format, GLenum type, unsigned char* data);

    void set_mip_face(GLenum face, unsigned int width, unsigned int height,
                      GLenum format, GLenum type, unsigned int mip_level,
                      unsigned char* data);

    void resize(unsigned int width, unsigned int height);

    void bind(int unit = -1);
    void unbind();
};

}  // namespace MR