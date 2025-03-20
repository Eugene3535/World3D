#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <filesystem>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "graphics/Color.hpp"


class Image
{
public:
    Image() noexcept;
    ~Image() noexcept;

    void create(uint32_t width, uint32_t height, const Color& color = Color(0, 0, 0)) noexcept;
    void create(uint32_t width, uint32_t height, const uint8_t* pixels) noexcept;

    bool loadFromFile(const std::filesystem::path& filepath) noexcept;
    bool saveToFile(const std::filesystem::path& filepath) const noexcept;

    glm::uvec2 getSize() const noexcept;

    void createMaskFromColor(const Color& color, uint8_t alpha = 0) noexcept;

    void copy(const Image& source, uint32_t destX, uint32_t destY, const glm::ivec4& sourceRect = glm::ivec4(0, 0, 0, 0), bool applyAlpha = false) noexcept;

    void setPixel(uint32_t x, uint32_t y, const Color& color) noexcept;
    Color getPixel(uint32_t x, uint32_t y) const noexcept;

    const uint8_t* getPixelPtr() const noexcept;

    void flipHorizontally() noexcept;
    void flipVertically() noexcept;

private:
    std::vector<uint8_t> m_pixels;
    glm::uvec2           m_size;
};

#endif // !IMAGE_HPP