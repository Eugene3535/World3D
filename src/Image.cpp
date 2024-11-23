#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Image.hpp"

Image::Image() noexcept:
    m_width(0),
    m_height(0),
    m_bytePerPixel(0)
{

}

Image::~Image() = default;

bool Image::loadFromFile(const std::filesystem::path& fPath) noexcept
{
    m_pixels.clear();

    uint8_t* data = stbi_load(fPath.generic_string().c_str(), &m_width, &m_height, &m_bytePerPixel, 0);

    if (!data)
        return false;

    m_pixels.resize(static_cast<size_t>((m_width * m_height) << 2));
    memcpy(m_pixels.data(), data, m_pixels.size());
    stbi_image_free(data);

    return true;
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