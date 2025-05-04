#include <array>
#include <memory>
#include <unordered_map>
#include <cstdio>

#include <glad/glad.h>
#include <SFML/Window/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image_write.h>
#include <utf8.h>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "files/Font.hpp"
#include "graphics/Glyph.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
#include "scenes/fonts/FontDemo.hpp"


FontDemo::FontDemo(sf::Window& window) noexcept:
    DemoScene(window),
    m_fontTexture(0)
{

}


FontDemo::~FontDemo()
{

}


bool FontDemo::init(GlResourceHolder& holder) noexcept
{
    auto [wndWidth, wndHeight] = m_window.getSize();

    auto vboHandles = holder.create<GlBuffer, 2>();
    auto vaoHandle = holder.create<VertexArrayObject, 1>();

    m_uniformBuffer = std::make_unique<GlBuffer>(vboHandles[0], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(glm::mat4));

    m_vbo = std::make_unique<GlBuffer>(vboHandles[1], GL_ARRAY_BUFFER);
    m_vbo->create(sizeof(glm::vec4), 4, nullptr, GL_DYNAMIC_DRAW);

    m_vao = std::make_unique<VertexArrayObject>(vaoHandle[0]);

    std::array<VertexBufferLayout::Attribute, 1> attributes
    {
        VertexBufferLayout::Attribute::Float4,
    };

    m_vao->addVertexBuffer(*m_vbo, attributes);

    m_camera = std::make_unique<OrthogonalCamera>();
    m_camera->flipVertically(false);
    m_camera->setupProjectionMatrix(wndWidth, wndHeight);

//  Shaders
    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("text.vert"), GL_VERTEX_SHADER))   false;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("text.frag"), GL_FRAGMENT_SHADER)) false;

    m_program = std::make_unique<ShaderProgram>();
    if(!m_program->link(shaders)) false;

//  Font
    m_font = std::make_unique<Font>();
    const uint32_t characterSize = 30;

    if(!m_font->loadFromFile("AvanteNrBook.ttf"))
        return false;

    if(!m_font->loadPage(characterSize))
        return false;

    m_page = m_font->getImage(characterSize);
    m_glyphs = m_font->getGlyphs(characterSize);

    if(!m_page.first || m_glyphs.empty())
        return false;

    auto utf8_to_wstring = [](const std::string& utf8_str) -> std::wstring
    {
        std::wstring wstr;
    
        if constexpr (sizeof(wchar_t) == 2) 
        {// UTF-16 (Windows)
            std::u16string utf16;
            utf8::utf8to16(utf8_str.begin(), utf8_str.end(), std::back_inserter(utf16));
            wstr.assign(utf16.begin(), utf16.end());
        } 
        else 
        {// UTF-32 (Linux/macOS)
            std::u32string utf32;
            utf8::utf8to32(utf8_str.begin(), utf8_str.end(), std::back_inserter(utf32));
            wstr.assign(utf32.begin(), utf32.end());
        }
    
        return wstr;
    };

    const char utf8Text[] = "Вау гречка ёЁ-юЮ-ыЫ first commit 1234 ()*<>q";
    m_text = utf8_to_wstring(utf8Text);

//  disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto texHandle = holder.create<Texture, 1>();
    m_fontTexture = texHandle[0];
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_page.second.x, m_page.second.y, 0, GL_RED, GL_UNSIGNED_BYTE, m_page.first);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

//  enable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
//  enable blending settings
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}


void FontDemo::update(const sf::Time& dt) noexcept
{
    m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(m_camera->getModelViewProjectionMatrix())));
}


void FontDemo::draw() noexcept
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program->getHandle());
    
    glm::vec3 color(0.5, 0.8f, 0.2f);
    renderText(m_text, color, 250.0f, 370.0f);

    glUseProgram(0);

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
}


void FontDemo::renderText(const std::wstring& text, const glm::vec3& color, float x, float y) noexcept
{
    if(auto uniform = glGetUniformLocation(m_program->getHandle(), "textColor"); uniform != -1)
        glUniform3f(uniform, color.x, color.y, color.z);

    glBindTextureUnit(0, m_fontTexture);
    glBindVertexArray(m_vao->getHandle());

    // iterate through all characters
    for (auto wc : text)
    {
        const Glyph& glyph = m_glyphs[wc];

        float xpos = x + glyph.bearing.x;
        float ypos = y - (glyph.size.y - glyph.bearing.y);

        float w = glyph.size.x;
        float h = glyph.size.y;

        float left   = glyph.textureRect.x;
        float top    = glyph.textureRect.y;
        float right  = glyph.textureRect.z;
        float bottom = glyph.textureRect.w;

        // update VBO for each character
        float vertices[16] = 
        {
            xpos,     ypos,     left, bottom,
            xpos + w, ypos,     right, bottom,
            xpos + w, ypos + h, right, top,
            xpos,     ypos + h, left, top
        };

        // update content of VBO memory
        m_vbo->update(0, sizeof(glm::vec4), 4, vertices);

        // render quad
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (glyph.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);
    glBindTextureUnit(0, 0);
}