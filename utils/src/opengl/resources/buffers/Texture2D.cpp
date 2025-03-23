#include <cstring>

#include "files/Image.hpp"
#include "opengl/resources/buffers/Texture2D.hpp"


Texture2D::Texture2D(GLuint handle) noexcept:
    m_handle(handle),
    m_width(0),
    m_height(0),
    m_isSmooth(false),
    m_isRepeated(false)
{
    
}


bool Texture2D::loadFromImage(const Image& image, bool repeat, bool smooth) noexcept
{
    if(m_handle)
    {
        m_width  = image.getSize().x;
        m_height = image.getSize().y;

        GLint wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_BORDER;
        GLint filterMode = smooth  ? GL_LINEAR : GL_NEAREST;
        m_isRepeated = repeat;
        m_isSmooth   = smooth;

        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(image.getPixelPtr()));
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    return false;
}


bool Texture2D::loadFromFile(const std::filesystem::path& filePath, bool repeat, bool smooth) noexcept
{
    Image image;

    if (image.loadFromFile(filePath))
        return loadFromImage(image, repeat, smooth);
    
    return false;
}


bool Texture2D::create(GLuint width, GLuint height) noexcept
{
    if(width && height)
    {
        if (GLuint maxSize = getMaximumSize(); (width > maxSize) || (height > maxSize))
            return false;
        
        m_width = width;
        m_height = height;

        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    return false;
}


void Texture2D::update(GLuint x, GLuint y, GLuint width, GLuint height, const uint8_t* pixels) noexcept
{
//  Check out of bounds
    if(x + width  <= m_width)  return;
    if(y + height <= m_height) return;

    if (pixels)
    {
        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


bool Texture2D::copyToImage(Image& image) noexcept
{
    if(m_width && m_height)
    {
        std::vector<uint8_t> pixels((m_width * m_height) << 2);

        glBindTexture(GL_TEXTURE_2D, m_handle);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        image.create(m_width, m_height, pixels.data());

        return true;
    }

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


GLuint Texture2D::getMaximumSize() noexcept
{
    static bool checked = false;
    static GLint size = 0;

    if (!checked)
    {
        checked = true;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
    }

    return static_cast<GLuint>(size);
}