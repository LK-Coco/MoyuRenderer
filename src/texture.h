#pragma once

#include <string>
#include "glad/glad.h"

namespace MR {

class Texture {
public:
    unsigned int id;

    GLenum target = GL_TEXTURE_2D;
    GLenum internal_format = GL_RGBA;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    GLenum filter_min = GL_LINEAR_MIPMAP_LINEAR;
    GLenum filter_max = GL_LINEAR;
    GLenum wrap_s = GL_REPEAT;
    GLenum wrap_t = GL_REPEAT;
    GLenum wrap_r = GL_REPEAT;
    bool mipmapping = true;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int depth = 0;

    Texture(){};
    ~Texture(){};

    void generate(unsigned int w, GLenum internal_format, GLenum format, GLenum type, void* data);

    void generate(unsigned int w, unsigned int h, GLenum internal_format, GLenum format,
                  GLenum type, void* data);

    void generate(unsigned int w, unsigned int h, unsigned int d, GLenum internal_format,
                  GLenum format, GLenum type, void* data);

    void resize(unsigned int w, unsigned int h = 0, unsigned int d = 0);

    void bind(int unit = -1);
    void unbind();

    void set_wrap_mode(GLenum wrap_mode, bool is_bind = false);
    void set_filter_min(GLenum filter, bool is_bind = false);
    void set_filter_max(GLenum filter, bool is_bind = false);
};

}  // namespace MR