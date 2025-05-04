#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <filesystem>
#include <memory>

#include "Export.hpp"

struct SHINY_API StbImage
{
    struct SHINY_API StbImageDeleter 
    {
        void operator()(uint8_t* src) noexcept;
    };

    bool loadFromFile(const std::filesystem::path& filepath) noexcept;

    std::unique_ptr<uint8_t[], StbImageDeleter> pixels;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bytePerPixel = 0;
};

#endif // !IMAGE_HPP