#include <glad/glad.h>

#include "Image.hpp"
#include "Texture.hpp"

uint32_t Texture::createFromImage(const Image& image, int32_t wrap, int32_t filter) noexcept
{
    uint32_t handle;
    GLint channels = (image.getBytePerPixel() == 4) ? GL_RGBA: GL_RGB;

	glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexImage2D(GL_TEXTURE_2D, 0, channels, image.getWidth(), image.getHeight(), 0, channels, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(image.getPixels()));
	glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return handle;
}