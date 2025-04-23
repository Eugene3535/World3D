#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "files/StbImage.hpp"

void StbImage::StbImageDeleter::operator()(uint8_t* src) noexcept
{
    stbi_image_free(src);
}

bool StbImage::loadFromFile(const std::filesystem::path& filepath) noexcept
{
    if (stbi_uc* src = stbi_load(filepath.generic_string().c_str(), &width, &height, &bytePerPixel, 0); src != nullptr)
    {
        pixels.reset(src);

        return true;
    }

    return false;
}