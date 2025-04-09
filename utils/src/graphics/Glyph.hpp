#ifndef GLYPH_HPP
#define GLYPH_HPP

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


struct Glyph
{
    glm::ivec2 bearing;     // Offset from baseline to left/top of glyph
    glm::ivec2 size;        // Size of glyph
    glm::ivec4 textureRect; // Texture coordinates of the glyph inside the font's texture
    uint32_t advance = 0;   // Horizontal offset to advance to next glyph
};

using GlyphTable = std::unordered_map<wchar_t, Glyph>;

#endif // !GLYPH_HPP