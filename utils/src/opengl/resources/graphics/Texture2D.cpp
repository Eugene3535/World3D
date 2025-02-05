#include <cstring>

#include <glad/glad.h>

#include "files/Image.hpp"
#include "opengl/resources/graphics/Texture2D.hpp"


static constexpr auto wrapModeToGlType(Texture2D::WrapMode mode) noexcept
{
    switch (mode)
    {
        case Texture2D::WrapMode::Repeat:        return GL_REPEAT;
        case Texture2D::WrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;

        default: return GL_REPEAT;
    }
}


static constexpr auto filterModeToGlType(Texture2D::FilterMode mode) noexcept
{
    switch (mode)
    {
        case Texture2D::FilterMode::Nearest: return GL_NEAREST;
        case Texture2D::FilterMode::Linear:  return GL_LINEAR;

        default: return GL_NEAREST;
    }
}


Texture2D::Texture2D(uint32_t handle) noexcept:
    GlResource(handle),
    m_width(0),
    m_height(0),
    m_isSmooth(false),
    m_isRepeated(false)
{
    
}


Texture2D::~Texture2D() noexcept = default;


bool Texture2D::loadFromImage(const Image& image, WrapMode wrap, FilterMode filter) noexcept
{
    if (int32_t channels = image.getBytePerPixel(); (channels == 3) || (channels == 4))
    {
        int32_t format = (channels == 4) ? GL_RGBA : GL_RGB;
		m_width = image.getWidth();
		m_height = image.getHeight();
        m_isRepeated = (wrap == WrapMode::Repeat) ? true : false;
        m_isSmooth = (filter == FilterMode::Linear) ? true : false;

        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeToGlType(wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeToGlType(wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterModeToGlType(filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterModeToGlType(filter));
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(image.getPixels()));
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    return false;
}


bool Texture2D::loadFromFile(const std::filesystem::path& filePath, WrapMode wrap, FilterMode filter) noexcept
{
    Image image;

    if (image.loadFromFile(filePath))
        return loadFromImage(image, wrap, filter);
    
    return false;
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


int32_t Texture2D::getWidth() const noexcept
{
    return m_width;
}


int32_t Texture2D::getHeight() const noexcept
{
    return m_height;
}


void Texture2D::bind(const Texture2D* texture) noexcept
{
    glBindTexture(GL_TEXTURE_2D, (texture != nullptr) ? texture->m_handle : 0);
}


void Texture2D::enable(uint32_t unit) noexcept
{
    glActiveTexture(GL_TEXTURE0 + unit);
}