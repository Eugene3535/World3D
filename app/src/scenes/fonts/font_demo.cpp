#include <array>
#include <memory>
#include <unordered_map>
#include <codecvt>
#include <cstdio>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image_write.h>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "graphics/Glyph.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
#include "files/Font.hpp"


static void renderText(GlyphTable& glyphs, VertexArrayObject& vao, GlBuffer& vbo, const std::wstring& text, float x, float y) noexcept
{
    glBindVertexArray(vao.getHandle());

    // iterate through all characters
    for (auto wc : text)
    {
        const Glyph& glyph = glyphs[wc];

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
        vbo.update(0, sizeof(glm::vec4), 4, vertices);

        // render quad
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (glyph.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);
    glUseProgram(0);
}



int font_demo(sf::Window& window) noexcept
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto [wndWidth, wndHeight] = window.getSize();

    GlResourceHolder resourceHolder;
    auto vboHandles = resourceHolder.create<GlBuffer, 2>();
    auto vaoHandle = resourceHolder.create<VertexArrayObject, 1>();

    GlBuffer uniformBuffer(vboHandles[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    GlBuffer vbo(vboHandles[1], GL_ARRAY_BUFFER);
    vbo.create(sizeof(glm::vec4), 4, nullptr, GL_DYNAMIC_DRAW);

    VertexArrayObject vao(vaoHandle[0]);
    std::array<VertexBufferLayout::Attribute, 1> attributes
    {
        VertexBufferLayout::Attribute::Float4,
    };
    vao.addVertexBuffer(vbo, attributes);

    auto orthoCamera = std::make_unique<OrthogonalCamera>();
    orthoCamera->flipVertically(false);
    orthoCamera->setupProjectionMatrix(wndWidth, wndHeight);

//  Shaders
    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("text.vert"), GL_VERTEX_SHADER))   return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("text.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return -1;

//  Font
    Font font;
    const uint32_t characterSize = 30;

    if(!font.loadFromFile("AvanteNrBook.ttf"))
        return -1;

    if(!font.loadPage(characterSize))
        return -1;

    auto page = font.getImage(characterSize);
    auto glyphs = font.getGlyphs(characterSize);

    if(glyphs.empty())
        return -1;

    const char utf8[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя0123456789!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~ ";
    const std::wstring utf16(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8));

    const char utf8Text[] = "Вау гречка ёЁ-юЮ-ыЫ first commit 1234 ()*<>q";
    std::wstring utf16Text(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8Text));

//  disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto texHandle = resourceHolder.create<Texture, 1>();
    GLuint texture = texHandle[0];
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, page.second.x, page.second.y, 0, GL_RED, GL_UNSIGNED_BYTE, page.first);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // enable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::Resized)
            {
                wndWidth = event.size.width;
                wndHeight = event.size.height;
                orthoCamera->setupProjectionMatrix(wndWidth, wndHeight);
                glViewport(0, 0, wndWidth, wndHeight);
            }
        }

        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(orthoCamera->getModelViewProjectionMatrix())));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        auto shader = program->getHandle().value();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(shader);
    
        glm::vec3 color(0.5, 0.8f, 0.2f);

        if(auto u = glGetUniformLocation(shader, "textColor"); u != -1)
            glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);

        renderText(glyphs, vao, vbo, utf16Text, 250.0f, 370.0f);

        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        window.display();
    }

    return 0;
}