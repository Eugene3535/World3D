#ifndef FONT_DEMO_HPP
#define FONT_DEMO_HPP

#include <memory>
#include <string>
#include <utility>

#include "files/Font.hpp"
#include "scenes/DemoScene.hpp"


class FontDemo final:
    public DemoScene
{
public:
    FontDemo(class sf::Window& window) noexcept;
    ~FontDemo();

    bool init() noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

private:
    void renderText(const std::wstring& text, const vec3 color, float x, float y) noexcept;

    std::unique_ptr<class ShaderProgram>     m_program;
    std::unique_ptr<class GlBuffer>          m_vbo;
    std::unique_ptr<class VertexArrayObject> m_vao;
    std::unique_ptr<class OrthogonalCamera>  m_camera;
    std::unique_ptr<class GlBuffer>          m_uniformBuffer;
    std::unique_ptr<Font>                    m_font;
    GlyphTable                               m_glyphs;
    Font::Image                              m_page;
    std::wstring                             m_text;
    uint32_t                                 m_fontTexture;
};


#endif // !FONT_DEMO_HPP