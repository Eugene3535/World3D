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

Image::~Image() noexcept = default;

bool Image::loadFromFile(const std::filesystem::path& fPath) noexcept
{
    if(!m_pixels.empty())
        m_pixels.clear();

    stbi_set_flip_vertically_on_load(1);

    if(stbi_uc* data = stbi_load(fPath.generic_string().c_str(), &m_width, &m_height, &m_bytePerPixel, 0); data)
    {
        const std::string& extension = fPath.extension().string();

        if(extension == ".jpg")
        {
            m_format = Format::JPEG;
        }
        else if(extension == ".png")
        {
            m_format = Format::PNG;
        }
        else if(extension == ".bmp")
        {
            m_format = Format::BMP;
        }
        else if(extension == ".psd")
        {
            m_format = Format::PSD;
        }
        else if(extension == ".tga")
        {
            m_format = Format::TGA;
        }
        else // Unsupported format
        {
            stbi_image_free(data);

            return false;
        }

        m_pixels.resize(static_cast<size_t>(m_width * m_height * m_bytePerPixel));
        memcpy(m_pixels.data(), data, m_pixels.size());

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