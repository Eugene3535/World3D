#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <filesystem>
#include <vector>

#include "Export.hpp"

class RE_API Image
{
public:
    enum class Format: int32_t
    {
        NONE = -1,
        JPEG,
        PNG,
        BMP,
        PSD,
        TGA
    };

    Image() noexcept;
    ~Image() noexcept;

    bool loadFromFile(const std::filesystem::path& fPath) noexcept;

    const uint8_t* getPixels()       const noexcept;
    int32_t        getWidth()        const noexcept;
    int32_t        getHeight()       const noexcept;
    int32_t        getBytePerPixel() const noexcept;
    Format         getFormat()       const noexcept;

private:
    std::vector<uint8_t> m_pixels;
    int32_t m_width;
    int32_t m_height;
    int32_t m_bytePerPixel;
    Format  m_format;
};

#endif // !IMAGE_HPP