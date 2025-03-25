#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/String.hpp>

#include "graphics/Color.hpp"


class Text
{
public:
    enum Style: uint32_t
    {
        Regular       = 0,
        Bold          = 1 << 0,
        Italic        = 1 << 1,
        Underlined    = 1 << 2,
        StrikeThrough = 1 << 3
    };

    Text() noexcept;
    Text(const sf::String& string, const class Font* font, uint32_t characterSize = 30) noexcept;

    void setString(const sf::String& string)   noexcept;
    void setFont(const class Font* font)       noexcept;
    void setCharacterSize(uint32_t size)       noexcept;
    void setLineSpacing(float spacingFactor)   noexcept;
    void setLetterSpacing(float spacingFactor) noexcept;
    void setStyle(Style style)                 noexcept;
    void setFillColor(const Color& color)      noexcept;
    void setOutlineColor(const Color& color)   noexcept;
    void setOutlineThickness(float thickness)  noexcept;

    const sf::String& getString()                    const noexcept;
    const class Font* getFont()                      const noexcept;
    uint32_t          getCharacterSize()             const noexcept;
    float             getLetterSpacing()             const noexcept;
    float             getLineSpacing()               const noexcept;
    Style             getStyle()                     const noexcept;
    const Color&      getFillColor()                 const noexcept;
    const Color&      getOutlineColor()              const noexcept;
    float             getOutlineThickness()          const noexcept;
    sf::Vector2f      findCharacterPos(size_t index) const noexcept;

private:
    void ensureGeometryUpdate() const noexcept;

    sf::String          m_string;              //!< String to display
    const Font*         m_font;                //!< Font used to display the string
    uint32_t            m_characterSize;       //!< Base size of characters, in pixels
    float               m_letterSpacingFactor; //!< Spacing factor between letters
    float               m_lineSpacingFactor;   //!< Spacing factor between lines
    Style               m_style;               //!< Text style (see Style enum)
    Color               m_fillColor;           //!< Text fill color
    Color               m_outlineColor;        //!< Text outline color
    float               m_outlineThickness;    //!< Thickness of the text's outline
    //mutable VertexArray m_vertices;            //!< Vertex array containing the fill geometry
    //mutable VertexArray m_outlineVertices;     //!< Vertex array containing the outline geometry
    mutable sf::FloatRect   m_bounds;              //!< Bounding rectangle of the text (in local coordinates)
    mutable bool        m_geometryNeedUpdate;  //!< Does the geometry need to be recomputed?
    //mutable Uint64      m_fontTextureId;       //!< The font texture id
};

#endif // !TEXT_HPP