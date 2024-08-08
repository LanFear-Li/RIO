#include "texture.hpp"

#include <glad/glad.h>

Texture::Texture(bool is_srgb, uint32_t width, uint32_t height, uint32_t nrComponents, const void *data)
{
    glGenTextures(1, &id);

    GLenum format;
    if (nrComponents == 1) {
        format = GL_RED;
    } else if (nrComponents == 3) {
        format = GL_RGB;
    } else if (nrComponents == 4) {
        format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}
