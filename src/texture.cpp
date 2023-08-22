#include "texture.h"
#include "glad/glad.h"
#include "stb_image.h"

namespace MR {

Texture::Texture(const char* file_path) {
    id = -1;
    glGenTextures(1, &id);

    int width, height, nr_comp;
    unsigned char* data = stbi_load(file_path, &width, &height, &nr_comp, 0);
    if (data) {
        GLenum format = NULL;
        if (nr_comp == 1)
            format = GL_RED;
        else if (nr_comp == 3)
            format = GL_RGB;
        else if (nr_comp == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    stbi_image_free(data);
}

}  // namespace MR