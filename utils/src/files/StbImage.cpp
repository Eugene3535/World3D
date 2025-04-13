#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "files/StbImage.hpp"

bool StbImage::loadFromFile(const std::filesystem::path& filepath) noexcept
{
    if(!pixels.empty())
        pixels.clear();

    stbi_uc* src = stbi_load(filepath.generic_string().c_str(), &width, &height, &bytePerPixel, 0);

    if (src)
    {
        pixels.resize(width * height * bytePerPixel);
        memcpy(pixels.data(), src, pixels.size());
        stbi_image_free(src);

        return true;
    }

    return false;
}

bool StbImage::saveFromFile(const std::filesystem::path& filepath) noexcept
{
    bool result = false;

    if(pixels.empty() || width < 1 || height < 1 || bytePerPixel < 1 || bytePerPixel > 4)
        return result;

    result = stbi_write_png(filepath.generic_string().c_str(), width, height, bytePerPixel, pixels.data(), 0);

    return result;
}