#include <cstring>

#include "files/StbImage.hpp"
#include "opengl/resources/buffers/Texture2D.hpp"


Texture2D::Texture2D(GLuint handle) noexcept:
    m_handle(handle),
    m_width(0),
    m_height(0),
    m_isSmooth(false),
    m_isRepeated(false)
{
    
}


bool Texture2D::loadFromImage(const StbImage& image, bool repeat, bool smooth) noexcept
{
    if(m_handle)
    {
        m_width  = static_cast<GLuint>(image.width);
        m_height = static_cast<GLuint>(image.height);

        GLint format = GL_RGBA;
        if(image.bytePerPixel == 1) format = GL_RED;
        if(image.bytePerPixel == 3) format = GL_RGB;

        GLint wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_BORDER;
        GLint filterMode = smooth  ? GL_LINEAR : GL_NEAREST;
        m_isRepeated = repeat;
        m_isSmooth   = smooth;

        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexImage2D(GL_TEXTURE_2D, 0, format, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height), 0, format, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(image.pixels.data()));
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    return false;
}


bool Texture2D::loadFromFile(const std::filesystem::path& filePath, bool repeat, bool smooth) noexcept
{
    StbImage image;

    if (image.loadFromFile(filePath))
        return loadFromImage(image, repeat, smooth);
    
    return false;
}

GLuint Texture2D::getHandle() const noexcept
{
    return m_handle;
}

GLuint Texture2D::getWidth() const noexcept
{
    return m_width;
}

GLuint Texture2D::getHeight() const noexcept
{
    return m_height;
}

void Texture2D::setSmooth(bool smooth) noexcept
{
    if(m_handle)
    {
        if(m_isSmooth != smooth)
        {
            m_isSmooth = smooth;

            glBindTexture(GL_TEXTURE_2D, m_handle);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

void Texture2D::setRepeated(bool repeate) noexcept
{
    if(m_handle)
    {
        if(m_isRepeated != repeate)
        {
            m_isRepeated = repeate;
            glBindTexture(GL_TEXTURE_2D, m_handle);

            if (m_isRepeated)
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

bool Texture2D::isSmooth() const noexcept
{
    return m_isSmooth;
}

bool Texture2D::isRepeated() const noexcept
{
    return m_isRepeated;
}