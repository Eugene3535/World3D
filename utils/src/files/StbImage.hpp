#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <filesystem>
#include <vector>

struct StbImage
{
    bool loadFromFile(const std::filesystem::path& filepath) noexcept;
    bool saveFromFile(const std::filesystem::path& filepath) noexcept;

    std::vector<uint8_t> pixels;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bytePerPixel = 0;
};

#endif // !IMAGE_HPP