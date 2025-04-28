#ifndef GLYPH_HPP
#define GLYPH_HPP

#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


struct RENDERER_API Glyph
{
    glm::ivec2 bearing;     // Offset from baseline to left/top of glyph
    glm::ivec2 size;        // Size of glyph
    glm::ivec4 imageRect;   // Coordinates of the glyph inside the font's image
    glm::vec4  textureRect; // Texture coordinates of the glyph inside the font's texture as left - top - right - bottom
    uint32_t advance = 0;   // Horizontal offset to advance to next glyph
};

using GlyphTable = std::unordered_map<wchar_t, Glyph>;

#endif // !GLYPH_HPP