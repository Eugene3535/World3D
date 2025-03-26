#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include <vector>

#include "glad/glad.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/String.hpp>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "graphics/Transform2D.hpp"
#include "graphics/Color.hpp"


class Text: public Transform2D
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
    ~Text() noexcept;

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

    void draw() noexcept;

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
    mutable glm::vec4   m_bounds;              //!< Bounding rectangle of the text (in local coordinates)
    mutable bool        m_geometryNeedUpdate;  //!< Does the geometry need to be recomputed?
    mutable const class Texture2D* m_fontTextureId = nullptr;       //!< The font texture id


    GLuint m_vboText = 0;
    GLuint m_vboOutline = 0;
    GLuint m_vaoText = 0;
    GLuint m_vaoOutline = 0;

    mutable GLsizei m_count = 0;
};

#endif // !TEXT_HPP