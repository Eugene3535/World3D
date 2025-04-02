#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "files/Image.hpp"


Image::Image() noexcept:
    m_size(0u, 0u)
{

}


Image::~Image() noexcept = default;


void Image::create(uint32_t width, uint32_t height, const Color& color) noexcept
{
    if (width && height)
    {
        std::vector<uint8_t> newPixels(width * height * 4);
    
        uint8_t* ptr = newPixels.data();
        uint8_t* end = ptr + newPixels.size();

        while (ptr < end)
        {
            *ptr++ = color.r;
            *ptr++ = color.g;
            *ptr++ = color.b;
            *ptr++ = color.a;
        }
    
        m_pixels.swap(newPixels);
        m_size.x = width;
        m_size.y = height;
    }
    else
    {
        std::vector<uint8_t>().swap(m_pixels);
        m_size.x = 0;
        m_size.y = 0;
    }
}


void Image::create(uint32_t width, uint32_t height, const uint8_t* pixels) noexcept
{
    if (pixels && width && height)
    {
        std::vector<uint8_t> newPixels(pixels, pixels + width * height * 4);
        m_pixels.swap(newPixels);
        m_size.x = width;
        m_size.y = height;
    }
    else
    {
        std::vector<uint8_t>().swap(m_pixels);
        m_size.x = 0;
        m_size.y = 0;
    }
}


bool Image::loadFromFile(const std::filesystem::path& filepath) noexcept
{
    m_pixels.clear();

    int32_t width = 0;
    int32_t height = 0;
    int32_t bytePerPixel = 0;
    stbi_uc* data = stbi_load(filepath.generic_string().c_str(), &width, &height, &bytePerPixel, STBI_rgb_alpha);

    if (data)
    {
        create(width, height, data);
        stbi_image_free(data);

        return true;
    }

    return false;
}


bool Image::saveToFile(const std::filesystem::path& filepath) const noexcept
{
    if ((!m_pixels.empty()) && (m_size.x > 0u) && (m_size.y > 0u))
    {
        std::string extension = filepath.extension().generic_string();
        int32_t width  = static_cast<int32_t>(m_size.x);
        int32_t height = static_cast<int32_t>(m_size.y);

        if (extension == ".bmp")
        {
            return stbi_write_bmp(filepath.generic_string().c_str(), width, height, 4, m_pixels.data()) != 0;
        }
        else if (extension == ".tga")
        {     
            return stbi_write_tga(filepath.generic_string().c_str(), width, height, 4, m_pixels.data()) != 0;
        }
        else if (extension == ".png")
        {        
            return stbi_write_png(filepath.generic_string().c_str(), width, height, 4, m_pixels.data(), 0) != 0;
        }
        else if (extension == ".jpg" || extension == ".jpeg")
        {     
            return stbi_write_jpg(filepath.generic_string().c_str(), width, height, 4, m_pixels.data(), 90) != 0;
        }
    }

    return false;
}


const glm::uvec2& Image::getSize() const noexcept
{
    return m_size;
}


void Image::createMaskFromColor(const Color& color, uint8_t alpha) noexcept
{
    if (!m_pixels.empty())
    {
        uint8_t* ptr = m_pixels.data();
        uint8_t* end = ptr + m_pixels.size();

        while (ptr < end)
        {
            if ((ptr[0] == color.r) && (ptr[1] == color.g) && (ptr[2] == color.b) && (ptr[3] == color.a))
                ptr[3] = alpha;

            ptr += 4;
        }
    }
}


void Image::copy(const Image& source, uint32_t destX, uint32_t destY, const glm::ivec4& sourceRect, bool applyAlpha) noexcept
{
    if ((source.m_size.x == 0) || (source.m_size.y == 0) || (m_size.x == 0) || (m_size.y == 0))
        return;

    glm::ivec4 srcRect = sourceRect;

    if (srcRect.z == 0 || (srcRect.w == 0))
    {
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.z = static_cast<int32_t>(source.m_size.x);
        srcRect.w = static_cast<int32_t>(source.m_size.y);
    }
    else
    {
        if (srcRect.x < 0) srcRect.x = 0;
        if (srcRect.y < 0) srcRect.y  = 0;
        if (srcRect.z > static_cast<int32_t>(source.m_size.x)) srcRect.z = static_cast<int32_t>(source.m_size.x);
        if (srcRect.w > static_cast<int32_t>(source.m_size.y)) srcRect.w = static_cast<int32_t>(source.m_size.y);
    }

    int32_t width  = static_cast<uint32_t>(srcRect.z);
    int32_t height = static_cast<uint32_t>(srcRect.w);

    if (destX + width  > m_size.x) width  = m_size.x - destX;
    if (destY + height > m_size.y) height = m_size.y - destY;

    if ((width <= 0) || (height <= 0))
        return;

    size_t pitch  = static_cast<size_t>(width) * 4;
    uint32_t rows = height;
    int32_t         srcStride = static_cast<int32_t>(source.m_size.x) * 4;
    int32_t         dstStride = static_cast<int32_t>(m_size.x) * 4;
    const uint8_t*  srcPixels = source.m_pixels.data() + ((static_cast<uint32_t>(srcRect.x) + static_cast<uint32_t>(srcRect.y) * source.m_size.x) << 2);
    uint8_t*        dstPixels = m_pixels.data() + ((destX + destY * m_size.x) << 2);

    if (applyAlpha)
    {
//      Interpolation using alpha values, pixel by pixel (slower)
        for (uint32_t i = 0; i < rows; ++i)
        {
            for (uint32_t j = 0; j < width; ++j)
            {
                const uint8_t* src = srcPixels + (j << 2);
                uint8_t*       dst = dstPixels + (j << 2);

                uint8_t src_alpha = src[3];
                uint8_t dst_alpha = dst[3];
                uint8_t out_alpha = static_cast<uint8_t>(src_alpha + dst_alpha - src_alpha * dst_alpha / 255);

                dst[3] = out_alpha;

                if (out_alpha)
                    for (int32_t k = 0; k < 3; k++)
                        dst[k] = static_cast<uint8_t>((src[k] * src_alpha + dst[k] * (out_alpha - src_alpha)) / out_alpha);
                else
                    for (int32_t k = 0; k < 3; k++)
                        dst[k] = src[k];
            }

            srcPixels += srcStride;
            dstPixels += dstStride;
        }
    }
    else
    {
//      Optimized copy ignoring alpha values, row by row (faster)
        for (uint32_t i = 0; i < rows; ++i)
        {
            memcpy(dstPixels, srcPixels, pitch);
            srcPixels += srcStride;
            dstPixels += dstStride;
        }
    }
}


void Image::setPixel(uint32_t x, uint32_t y, const Color& color) noexcept
{
    uint8_t* pixel = m_pixels.data() + ((x + y * m_size.x) << 2);

    *pixel++ = color.r;
    *pixel++ = color.g;
    *pixel++ = color.b;
    *pixel++ = color.a;
}


Color Image::getPixel(uint32_t x, uint32_t y) const noexcept
{
    const uint8_t* pixel = m_pixels.data() + ((x + y * m_size.x) << 2);

    return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
}


const uint8_t* Image::getPixelPtr() const noexcept
{
    return m_pixels.data();
}


void Image::flipHorizontally() noexcept
{
    if (!m_pixels.empty())
    {
        size_t rowSize = m_size.x * 4;

        for (size_t y = 0; y < m_size.y; ++y)
        {
            std::vector<uint8_t>::iterator left = m_pixels.begin() + static_cast<std::vector<uint8_t>::iterator::difference_type>(y * rowSize);
            std::vector<uint8_t>::iterator right = m_pixels.begin() + static_cast<std::vector<uint8_t>::iterator::difference_type>((y + 1) * rowSize - 4);

            for (size_t x = 0; x < m_size.x / 2; ++x)
            {
                std::swap_ranges(left, left + 4, right);

                left += 4;
                right -= 4;
            }
        }
    }
}


void Image::flipVertically() noexcept
{
    if (!m_pixels.empty())
    {
        auto rowSize = static_cast<std::vector<uint8_t>::iterator::difference_type>(m_size.x * 4);

        std::vector<uint8_t>::iterator top = m_pixels.begin();
        std::vector<uint8_t>::iterator bottom = m_pixels.end() - rowSize;

        for (size_t y = 0; y < m_size.y / 2; ++y)
        {
            std::swap_ranges(top, top + rowSize, bottom);

            top += rowSize;
            bottom -= rowSize;
        }
    }
}