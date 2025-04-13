#include <cstring>

#include <glad/glad.h>

#include "files/StbImage.hpp"
#include "opengl/resources/buffers/Texture.hpp"

bool Texture::loadFromImage(const StbImage& image, bool repeat, bool smooth) noexcept
{
    if(handle)
    {
        width  = static_cast<GLuint>(image.width);
        height = static_cast<GLuint>(image.height);

        GLint format = GL_RGBA;
        if(image.bytePerPixel == 1) format = GL_RED;
        if(image.bytePerPixel == 3) format = GL_RGB;

        GLint wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_BORDER;
        GLint filterMode = smooth  ? GL_LINEAR : GL_NEAREST;
        isRepeated = repeat;
        isSmooth   = smooth;

        glBindTexture(GL_TEXTURE_2D, handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexImage2D(GL_TEXTURE_2D, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(image.pixels.data()));
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    return false;
}

bool Texture::loadFromFile(const std::filesystem::path& filePath, bool repeat, bool smooth) noexcept
{
    StbImage image;

    if (image.loadFromFile(filePath))
        return loadFromImage(image, repeat, smooth);
    
    return false;
}

void Texture::setSmooth(bool smooth) noexcept
{
    if(handle)
    {
        if(isSmooth != smooth)
        {
            isSmooth = smooth;

            glBindTexture(GL_TEXTURE_2D, handle);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isSmooth ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isSmooth ? GL_LINEAR : GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

void Texture::setRepeated(bool repeate) noexcept
{
    if(handle)
    {
        if(isRepeated != repeate)
        {
            isRepeated = repeate;
            glBindTexture(GL_TEXTURE_2D, handle);

            if (isRepeated)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                constexpr static float border_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}