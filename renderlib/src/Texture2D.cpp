#include <cstring>

#include <glad/glad.h>

#include "Image.hpp"
#include "Texture2D.hpp"

Texture2D::Texture2D(const std::filesystem::path& filePath, int32_t wrap, int32_t filter) noexcept:
    m_handle(0u),
    m_width(0),
    m_height(0),
    m_isSmooth(false),
    m_isRepeated(false)
{
    static_assert(std::is_same_v<GLuint, uint32_t>, "GLuint and uint32_t are not the same type!");
    static_assert(std::is_same_v<GLenum, uint32_t>, "GLenum and uint32_t are not the same type!");
    static_assert(std::is_same_v<GLint, int32_t>, "GLint and int32_t are not the same type!");

    Image image;

    if (image.loadFromFile(filePath))
    {
        if (int32_t internalformat = image.getBytePerPixel(); (internalformat == 3) || (internalformat == 4))
        {
            uint32_t format = (internalformat == 4) ? GL_RGBA : GL_RGB;
            m_width = image.getWidth();
            m_height = image.getHeight();
            m_isSmooth = (filter == GL_LINEAR) ? true : false;
            m_isRepeated = (wrap == GL_REPEAT) ? true : false;

            glGenTextures(1, &m_handle);
            glBindTexture(GL_TEXTURE_2D, m_handle);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(image.getPixels()));
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}


Texture2D::Texture2D(const Image& image, int32_t wrap, int32_t filter) noexcept:
    m_handle(0u),
    m_width(0),
    m_height(0),
    m_isSmooth(false),
    m_isRepeated(false)
{
    static_assert(std::is_same_v<GLuint, uint32_t>, "GLuint and uint32_t are not the same type!");
    static_assert(std::is_same_v<GLenum, uint32_t>, "GLenum and uint32_t are not the same type!");
    static_assert(std::is_same_v<GLint, int32_t>, "GLint and int32_t are not the same type!");

    if (int32_t channels = image.getBytePerPixel(); (channels == 3) || (channels == 4))
    {
        int32_t format = (channels == 4) ? GL_RGBA : GL_RGB;
		m_width = image.getWidth();
		m_height = image.getHeight();
		m_isSmooth = (filter == GL_LINEAR) ? true : false;
		m_isRepeated = (wrap == GL_REPEAT) ? true : false;

        glGenTextures(1, &m_handle);
        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(image.getPixels()));
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


Texture2D::Texture2D(Texture2D&& texture) noexcept :
    m_handle(texture.m_handle),
    m_width(texture.m_width),
    m_height(texture.m_height),
    m_isSmooth(texture.m_isSmooth),
    m_isRepeated(texture.m_isRepeated)
{
    texture.m_handle = 0;
}


Texture2D& Texture2D::operator = (Texture2D&& texture) noexcept
{
    std::memcpy(this, &texture, sizeof(Texture2D));
    texture.m_handle = 0;

    return *this;
}


Texture2D::~Texture2D() noexcept
{
    glDeleteTextures(1, &m_handle);
}


void Texture2D::setSmooth(bool smooth) noexcept
{
    if(m_isSmooth != smooth)
    {
        m_isSmooth = smooth;
        Texture2D::bind(this);

        if (m_isSmooth)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        Texture2D::bind(nullptr);
    }
}


void Texture2D::setRepeated(bool repeate) noexcept
{
    if(m_isRepeated != repeate)
    {
        m_isRepeated = repeate;
        Texture2D::bind(this);

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

        Texture2D::bind(nullptr);
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


bool Texture2D::isValid() const noexcept
{
    return m_handle != 0;
}


uint32_t Texture2D::getHandle() const noexcept
{
    return m_handle;
}


int32_t Texture2D::width() const noexcept
{
    return m_width;
}


int32_t Texture2D::height() const noexcept
{
    return m_height;
}


void Texture2D::bind(const Texture2D* texture) noexcept
{
    glBindTexture(GL_TEXTURE_2D, (texture != nullptr) ? texture->m_handle : 0);
}


void Texture2D::enable(uint32_t unit) noexcept
{
    glActiveTexture(unit);
}