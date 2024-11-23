#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <filesystem>
#include <vector>

class Image
{
public:
    Image() noexcept;
    ~Image();

    bool loadFromFile(const std::filesystem::path& fPath) noexcept;

    const uint8_t* getPixels() const noexcept;
    int32_t getWidth()  const noexcept;
    int32_t getHeight() const noexcept;
    int32_t getBytePerPixel() const noexcept;

private:
    std::vector<uint8_t> m_pixels;
    int32_t m_width;
    int32_t m_height;
    int32_t m_bytePerPixel;
};

#endif // !IMAGE_HPP