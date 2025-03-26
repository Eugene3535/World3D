#include <cmath>

#include "files/Font.hpp"
#include "opengl/resources/buffers/Texture2D.hpp"
#include "graphics/Text.hpp"


Text::Text() noexcept :
    m_string(),
    m_font(nullptr),
    m_characterSize(30),
    m_letterSpacingFactor(1.f),
    m_lineSpacingFactor(1.f),
    m_style(Text::Regular),
    m_fillColor(255, 255, 255),
    m_outlineColor(0, 0, 0),
    m_outlineThickness(0),
    m_geometryNeedUpdate(false)
{
//  text
    glGenBuffers(1, &m_vboText);
    glGenVertexArrays(1, &m_vaoText);

    glBindVertexArray(m_vboText);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboText);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (const void*)0);

    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Color), (const void*)(sizeof(glm::vec4)));

//  outline
    glGenBuffers(1, &m_vboOutline);
    glGenVertexArrays(1, &m_vaoOutline);

    glBindVertexArray(m_vaoOutline);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboOutline);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (const void*)0);

    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Color), (const void*)(sizeof(glm::vec4)));
}


Text::~Text() noexcept
{
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &m_vaoText);
    glDeleteBuffers(1, &m_vboText);

    glDeleteVertexArrays(1, &m_vaoOutline);
    glDeleteBuffers(1, &m_vboOutline);
}


void Text::setString(const sf::String& string) noexcept
{
    if (m_string != string)
    {
        m_string = string;
        m_geometryNeedUpdate = true;
    }
}


void Text::setFont(const Font* font) noexcept
{
    if (m_font != font)
    {
        m_font = font;
        m_geometryNeedUpdate = true;
    }
}


void Text::setCharacterSize(uint32_t size) noexcept
{
    if (m_characterSize != size)
    {
        m_characterSize = size;
        m_geometryNeedUpdate = true;
    }
}


void Text::setLetterSpacing(float spacingFactor) noexcept
{
    if (m_letterSpacingFactor != spacingFactor)
    {
        m_letterSpacingFactor = spacingFactor;
        m_geometryNeedUpdate = true;
    }
}


void Text::setLineSpacing(float spacingFactor) noexcept
{
    if (m_lineSpacingFactor != spacingFactor)
    {
        m_lineSpacingFactor = spacingFactor;
        m_geometryNeedUpdate = true;
    }
}


void Text::setStyle(Text::Style style) noexcept
{
    if (m_style != style)
    {
        m_style = style;
        m_geometryNeedUpdate = true;
    }
}


void Text::setFillColor(const Color& color) noexcept
{
    if (color != m_fillColor)
    {
        m_fillColor = color;

        if (!m_geometryNeedUpdate)
        {
            // for (size_t i = 0; i < m_vertices.getVertexCount(); ++i)
            //     m_vertices[i].color = m_fillColor;
        }
    }
}


void Text::setOutlineColor(const Color& color) noexcept
{
    if (color != m_outlineColor)
    {
        m_outlineColor = color;

        if (!m_geometryNeedUpdate)
        {
            // for (size_t i = 0; i < m_outlineVertices.getVertexCount(); ++i)
            //     m_outlineVertices[i].color = m_outlineColor;
        }
    }
}


void Text::setOutlineThickness(float thickness) noexcept
{
    if (thickness != m_outlineThickness)
    {
        m_outlineThickness = thickness;
        m_geometryNeedUpdate = true;
    }
}


const sf::String& Text::getString() const noexcept
{
    return m_string;
}


const Font* Text::getFont() const noexcept
{
    return m_font;
}


uint32_t Text::getCharacterSize() const noexcept
{
    return m_characterSize;
}


float Text::getLetterSpacing() const noexcept
{
    return m_letterSpacingFactor;
}


float Text::getLineSpacing() const noexcept
{
    return m_lineSpacingFactor;
}


Text::Style Text::getStyle() const noexcept
{
    return m_style;
}


const Color& Text::getFillColor() const noexcept
{
    return m_fillColor;
}


const Color& Text::getOutlineColor() const noexcept
{
    return m_outlineColor;
}


float Text::getOutlineThickness() const noexcept
{
    return m_outlineThickness;
}


void Text::draw() noexcept
{
    if(m_geometryNeedUpdate)
        ensureGeometryUpdate();

    glBindVertexArray(m_vaoText);
    glDrawArrays(GL_TRIANGLES, 0, m_count);
    glBindVertexArray(0);
}


void Text::ensureGeometryUpdate() const noexcept
{
//   Add an underline or strikethrough line to the vertex array
    auto addLine = [](std::vector<glm::vec4>& vertices, float lineLength, float lineTop, /*const Color& color,*/ float offset, float thickness, float outlineThickness = 0) -> void
    {
        float top = std::floor(lineTop + offset - (thickness / 2) + 0.5f);
        float bottom = top + std::floor(thickness + 0.5f);

        vertices.emplace_back(-outlineThickness, top - outlineThickness, 1, 1);
        vertices.emplace_back(lineLength + outlineThickness, top - outlineThickness, 1, 1);
        vertices.emplace_back(-outlineThickness, bottom + outlineThickness, 1, 1);
        vertices.emplace_back(-outlineThickness, bottom + outlineThickness, 1, 1);
        vertices.emplace_back(lineLength + outlineThickness, top - outlineThickness, 1, 1);
        vertices.emplace_back(lineLength + outlineThickness, bottom + outlineThickness, 1, 1);
    };

//   Add a glyph quad to the vertex array
    auto addGlyphQuad = [](std::vector<glm::vec4>& vertices, const glm::vec2& position, /*const Color& color,*/ const Glyph& glyph, float italicShear) -> void
    {
        float padding = 1.f;

        float left = glyph.bounds.x - padding;
        float top = glyph.bounds.y - padding;
        float right = glyph.bounds.x + glyph.bounds.z + padding;
        float bottom = glyph.bounds.y + glyph.bounds.w + padding;

        float u1 = static_cast<float>(glyph.textureRect.x) - padding;
        float v1 = static_cast<float>(glyph.textureRect.y) - padding;
        float u2 = static_cast<float>(glyph.textureRect.x + glyph.textureRect.z) + padding;
        float v2 = static_cast<float>(glyph.textureRect.y + glyph.textureRect.w) + padding;

        vertices.emplace_back(position.x + left - italicShear * top, position.y + top, u1, v1);
        vertices.emplace_back(position.x + right - italicShear * top, position.y + top, u2, v1);
        vertices.emplace_back(position.x + left - italicShear * bottom, position.y + bottom, u1, v2);
        vertices.emplace_back(position.x + left - italicShear * bottom, position.y + bottom, u1, v2);
        vertices.emplace_back(position.x + right - italicShear * top, position.y + top, u2, v1);
        vertices.emplace_back(position.x + right - italicShear * bottom, position.y + bottom, u2, v2);
    };

    if (!m_font)
        return;

     // Do nothing, if geometry has not changed and the font texture has not changed
    if (!m_geometryNeedUpdate && m_font->getTexture(m_characterSize) == m_fontTextureId)
        return;

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> outlineVertices;

     // Save the current fonts texture id
    m_fontTextureId = m_font->getTexture(m_characterSize);

     // Mark geometry as updated
    m_geometryNeedUpdate = false;

     // Clear the previous geometry
     // m_vertices.clear();
     // m_outlineVertices.clear();
    m_bounds = glm::vec4(0.f, 0.f, 0.f, 0.f);

     // No text: nothing to draw
    if (m_string.isEmpty())
        return;

    // Compute values related to the text style
    bool  isBold             = m_style & Bold;
    bool  isUnderlined       = m_style & Underlined;
    bool  isStrikeThrough    = m_style & StrikeThrough;
    float italicShear        = (m_style & Italic) ? 0.209f : 0.f; // 12 degrees in radians
    float underlineOffset    = m_font->getUnderlinePosition(m_characterSize);
    float underlineThickness = m_font->getUnderlineThickness(m_characterSize);

    // Compute the location of the strike through dynamically
    // We use the center point of the lowercase 'x' glyph as the reference
    // We reuse the underline thickness as the thickness of the strike through as well
    auto xBounds = m_font->getGlyph(L'x', m_characterSize, isBold).bounds;
    float strikeThroughOffset = xBounds.y + xBounds.w / 2.f;

    // Precompute the variables needed by the algorithm
    float whitespaceWidth = m_font->getGlyph(L' ', m_characterSize, isBold).advance;
    float letterSpacing   = ( whitespaceWidth / 3.f ) * ( m_letterSpacingFactor - 1.f );
    whitespaceWidth      += letterSpacing;
    float lineSpacing     = m_font->getLineSpacing(m_characterSize) * m_lineSpacingFactor;
    float x               = 0.f;
    float y               = static_cast<float>(m_characterSize);

    // Create one quad for each character
    float minX = static_cast<float>(m_characterSize);
    float minY = static_cast<float>(m_characterSize);
    float maxX = 0.f;
    float maxY = 0.f;
    uint32_t prevChar = 0;

    for (size_t i = 0; i < m_string.getSize(); ++i)
    {
        uint32_t curChar = m_string[i];

        // Skip the \r char to avoid weird graphical issues
        if (curChar == L'\r')
            continue;

        // Apply the kerning offset
        x += m_font->getKerning(prevChar, curChar, m_characterSize, isBold);

        // If we're using the underlined style and there's a new line, draw a line
        if (isUnderlined && (curChar == L'\n' && prevChar != L'\n'))
        {
            addLine(vertices, x, y, /*m_fillColor,*/ underlineOffset, underlineThickness);

            if (m_outlineThickness != 0)
                addLine(outlineVertices, x, y, /*m_outlineColor,*/ underlineOffset, underlineThickness, m_outlineThickness);
        }

        // If we're using the strike through style and there's a new line, draw a line across all characters
        if (isStrikeThrough && (curChar == L'\n' && prevChar != L'\n'))
        {
            addLine(vertices, x, y, /*m_fillColor,*/ strikeThroughOffset, underlineThickness);

            if (m_outlineThickness != 0)
                addLine(outlineVertices, x, y, /*m_outlineColor,*/ strikeThroughOffset, underlineThickness, m_outlineThickness);
        }

        prevChar = curChar;

        // Handle special characters
        if ((curChar == L' ') || (curChar == L'\n') || (curChar == L'\t'))
        {
            // Update the current bounds (min coordinates)
            minX = std::min(minX, x);
            minY = std::min(minY, y);

            switch (curChar)
            {
            case L' ':
                x += whitespaceWidth;
                break;
            case L'\t':
                x += whitespaceWidth * 4;
                break;
            case L'\n':
                y += lineSpacing;
                x = 0;
                break;
            }

            // Update the current bounds (max coordinates)
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);

            // Next glyph, no need to create a quad for whitespace
            continue;
        }

        // Apply the outline
        if (m_outlineThickness != 0)
        {
            const Glyph &glyph = m_font->getGlyph(curChar, m_characterSize, isBold, m_outlineThickness);

            // Add the outline glyph to the vertices
            addGlyphQuad(outlineVertices, glm::vec2(x, y), /*m_outlineColor,*/ glyph, italicShear);
        }

        // Extract the current glyph's description
        const Glyph &glyph = m_font->getGlyph(curChar, m_characterSize, isBold);

        // Add the glyph to the vertices
        addGlyphQuad(vertices, glm::vec2(x, y), /*m_fillColor,*/ glyph, italicShear);

        // Update the current bounds
        float left = glyph.bounds.x;
        float top = glyph.bounds.y;
        float right = glyph.bounds.x + glyph.bounds.z;
        float bottom = glyph.bounds.y + glyph.bounds.w;

        minX = std::min(minX, x + left - italicShear * bottom);
        maxX = std::max(maxX, x + right - italicShear * top);
        minY = std::min(minY, y + top);
        maxY = std::max(maxY, y + bottom);

        // Advance to the next character
        x += glyph.advance + letterSpacing;
    }

    // If we're using outline, update the current bounds
    if (m_outlineThickness != 0)
    {
        float outline = std::abs(std::ceil(m_outlineThickness));
        minX -= outline;
        maxX += outline;
        minY -= outline;
        maxY += outline;
    }

    // If we're using the underlined style, add the last line
    if (isUnderlined && (x > 0))
    {
        addLine(vertices, x, y, /*m_fillColor,*/ underlineOffset, underlineThickness);

        if (m_outlineThickness != 0)
            addLine(outlineVertices, x, y, /*m_outlineColor,*/ underlineOffset, underlineThickness, m_outlineThickness);
    }

    // If we're using the strike through style, add the last line across all characters
    if (isStrikeThrough && (x > 0))
    {
        addLine(vertices, x, y, /*m_fillColor,*/ strikeThroughOffset, underlineThickness);

        if (m_outlineThickness != 0)
            addLine(outlineVertices, x, y, /*m_outlineColor,*/ strikeThroughOffset, underlineThickness, m_outlineThickness);
    }

    // Update the bounding rectangle
    m_bounds.x = minX;
    m_bounds.y = minY;
    m_bounds.z = maxX - minX;
    m_bounds.w = maxY - minY;

    m_count = vertices.size();

    if(!vertices.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboText);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec4)), vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if(!outlineVertices.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboOutline);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec4)), vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
