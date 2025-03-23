#ifndef FONT_HPP
#define FONT_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>

#include <glm/vec4.hpp>


struct Glyph
{
    float      advance = 0.f;
    int32_t    lsbDelta = 0.f;
    int32_t    rsbDelta = 0.f; 
    glm::vec4  bounds;
    glm::ivec4 textureRect;
};


struct Row
{
    Row(uint32_t rowTop, uint32_t rowHeight) noexcept;

    uint32_t width; 
    uint32_t top;   
    uint32_t height;
};


struct Page
{
    Page(uint32_t handle, bool smooth) noexcept;

    std::unordered_map<size_t, Glyph> glyphs; 
    std::unique_ptr<class Texture2D>  texture;
    uint32_t                          nextRow;
    std::vector<Row>                  rows;   
};


class Font
{
public:
    struct Info
    {
        std::string family;
    };

    Font(class GlResourceHolder& holder) noexcept;
    Font(const Font&)              noexcept = delete;
    Font(Font&&)                   noexcept = delete;
    Font& operator =(const Font&)  noexcept = delete;
    Font& operator =(Font&&)       noexcept = delete;
    ~Font()                        noexcept;

    bool loadFromFile(const std::filesystem::path& filepath) noexcept;

    const Info&  getInfo() const noexcept;
    const Glyph& getGlyph(uint32_t codePoint, uint32_t charSize, bool bold, float outlineThickness = 0) const noexcept;
    bool         hasGlyph(uint32_t codePoint) const noexcept;

    float getKerning(uint32_t first, uint32_t second, uint32_t charSize, bool bold = false) const noexcept;

    float getLineSpacing(uint32_t charSize)        const noexcept;
    float getUnderlinePosition(uint32_t charSize)  const noexcept;
    float getUnderlineThickness(uint32_t charSize) const noexcept;

    const class Texture2D* getTexture(uint32_t charSize) const noexcept;

    void setSmooth(bool smooth) noexcept;
    bool isSmooth() const noexcept;

private:
    void cleanup() noexcept;

    Page&      loadPage(uint32_t charSize)                                                         const noexcept;
    Glyph      loadGlyph(uint32_t codePoint, uint32_t charSize, bool bold, float outlineThickness) const noexcept;
    glm::ivec4 findGlyphRect(Page& page, uint32_t width, uint32_t height)                          const noexcept;
    bool       setCurrentSize(uint32_t charSize)                                                   const noexcept;

    class GlResourceHolder& m_glResources;

    void* m_library;
    void* m_face;
    void* m_stroker;

    bool m_isSmooth;
    Info m_info;

    mutable std::unordered_map<uint32_t, Page> m_pages;
};

#endif // !FONT_HPP