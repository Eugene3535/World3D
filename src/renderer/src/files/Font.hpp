#ifndef FONT_HPP
#define FONT_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

#include "graphics/Glyph.hpp"

class RENDERER_API Font
{
public:
    struct Info
    {
        std::string family;
    };

    Font() noexcept;
    Font(const Font&) noexcept = delete;
    Font(Font&&) noexcept = delete;
    Font& operator = (const Font&) noexcept = delete;
    Font& operator = (Font&&) noexcept = delete;
    ~Font() noexcept;

    bool loadFromFile(const std::string& font_name) noexcept;
    bool writeToFile(uint32_t characterSize) const noexcept;
    bool loadPage(uint32_t characterSize) noexcept;
    bool isLoaded() const noexcept;

    std::pair<const uint8_t*, glm::ivec2> getImage(uint32_t characterSize) const noexcept;
    const Info& getInfo() const noexcept;

    bool hasGlyph(wchar_t codePoint) const noexcept;
    void getGlyph(wchar_t codePoint, uint32_t characterSize, Glyph* glyph) const noexcept;
    GlyphTable getGlyphs(uint32_t characterSize) const noexcept;

private:
    struct Row
    {
        uint32_t width  = 0;
        uint32_t top    = 0;
        uint32_t height = 0;
    };

    struct Page
    {
        GlyphTable           glyphs;
        std::vector<Row>     rows;
        uint32_t             nextRow = 0;
        std::vector<uint8_t> image;
        glm::ivec2           size;
    };

    void cleanup() noexcept;
    glm::ivec4 findGlyphRect(Page& page, uint32_t width, uint32_t height) noexcept;
    void writeGlyphToPage(wchar_t wc, Page& page) noexcept;

    static std::wstring utf16;
    
    std::unordered_map<uint32_t, Page> m_pages;

    Info m_info;

    void* m_library;
    void* m_face;
};

#endif // !FONT_HPP