#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Image.hpp"

Image::Image() noexcept:
    m_width(0),
    m_height(0),
    m_bytePerPixel(0),
    m_format(Format::NONE)
{

}

Image::~Image() = default;

bool Image::loadFromFile(const std::filesystem::path& fPath) noexcept
{
    if(!m_pixels.empty())
        m_pixels.clear();

    stbi_set_flip_vertically_on_load(1);

    if(uint8_t* data = stbi_load(fPath.generic_string().c_str(), &m_width, &m_height, &m_bytePerPixel, 0); data)
    {
        const std::string& extension = fPath.extension().string();

        //if(extension)

        m_pixels.resize(static_cast<size_t>(m_width * m_height * m_bytePerPixel));
        memcpy(m_pixels.data(), data, m_pixels.size());
        stbi_image_free(data);

        return true;
    }

    return false;
}

const uint8_t* Image::getPixels() const noexcept
{
    return m_pixels.data();
}

int32_t Image::getWidth() const noexcept
{
    return m_width;
}

int32_t Image::getHeight() const noexcept
{
    return m_height;
}

int32_t Image::getBytePerPixel() const noexcept
{
    return m_bytePerPixel;
}

Image::Format Image::getFormat() const noexcept
{
    return m_format;
}