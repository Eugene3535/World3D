#pragma once

#include <filesystem>
#include <memory>

struct StbImage
{
    enum Mode : int
    {
        Default = 0,
        Grey,
        GreyAlpha,
        Rgb,
        Alpha
    };

    struct StbImageDeleter 
    {
        void operator()(uint8_t* src) noexcept;
    };

    bool loadFromFile(const std::filesystem::path& filepath, int mode) noexcept;

    std::unique_ptr<uint8_t[], StbImageDeleter> pixels;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bytePerPixel = 0;
};