#include <array>
#include <memory>
#include <unordered_map>
#include <codecvt>
#include <cstdio>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stb_image_write.h>

#include "files/Image.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"

struct Glyph
{
    glm::ivec2 bearing;     // Offset from baseline to left/top of glyph
    glm::ivec2 size;        // Size of glyph
    glm::ivec4 textureRect; // Texture coordinates of the glyph inside the font's texture
    GLuint advance;         // Horizontal offset to advance to next glyph
};

struct Row
{
    uint32_t width;  // Current width of the row
    uint32_t top;    // Y position of the row into the texture
    uint32_t height; // Height of the row
};

// Holds all state information relevant to a character as loaded using FreeType
struct Character 
{
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Horizontal offset to advance to next glyph
    GLuint TextureID;   // ID handle of the glyph texture
};

using Characters = std::unordered_map<wchar_t, Character>;
struct Page
{
    Characters           characters; // Table mapping code points to their corresponding glyph
    std::vector<Row>     rows;       // List containing the position of all the existing rows
    uint32_t             nextRow;    // Y position of the next new row in the texture
    std::vector<uint8_t> pixels;     // Image handle containing the pixels of the glyphs
    glm::ivec2           size;       // Size of page in pixels
};


static void RenderText(Characters& characters, VertexArrayObject& vao, GlBuffer& vbo, const std::wstring& text, float x, float y) noexcept
{
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao.getHandle());

    // iterate through all characters
    for (auto c : text)
    {
        const Character& ch = characters[c];

        float xpos = x + ch.Bearing.x;
        float ypos = y - (ch.Size.y - ch.Bearing.y);

        float w = ch.Size.x;
        float h = ch.Size.y;

        // update VBO for each character
        float vertices[16] = 
        {
            xpos,     ypos + h, 0.f, 0.f,
            xpos,     ypos,     0.f, 1.f,
            xpos + w, ypos,     1.f, 1.f,
            xpos + w, ypos + h, 1.f, 0.f
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // update content of VBO memory
        vbo.update(0, sizeof(glm::vec4), 4, vertices);

        // render quad
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

bool needWrite = true;

void WriteGlyphToPage(Page& page, const FT_Bitmap& bitmap) noexcept
{
    uint32_t rows = bitmap.rows;
    uint32_t pitch = bitmap.pitch;

    const uint8_t* srcPixels = bitmap.buffer;
    uint8_t*       dstPixels = page.pixels.data();

    uint32_t pageStride = page.size.x;

    for (uint32_t i = 0; i < rows; ++i)
    {
        memcpy(dstPixels, srcPixels, pitch);
        srcPixels += pitch;
        dstPixels += pageStride;
    }

    if(needWrite)
    {
        needWrite = false;
        std::string fName = "char_0.png";
        stbi_write_png(fName.c_str(), page.size.x, page.size.y, 1, page.pixels.data(), 0);
    }
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
    FT_Library ftLibrary;
    FT_Face face;

    if (FT_Init_FreeType(&ftLibrary))
        return -1;

    if (FT_New_Face(ftLibrary, FileProvider::findPathToFile("AvanteNrBook.ttf").generic_string().c_str(), 0, &face))
        return -1;

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 30);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const char utf8[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя0123456789!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~ ";
    const std::wstring utf16(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8));

    const char utf8Text[] = "Вау гречка ёЁ-юЮ first commit 1234 ()*<>";
    std::wstring utf16Text(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8Text));

    Characters characters;

    // int cnt = 0;

    auto& bitmap = face->glyph->bitmap;

    Page page;
    page.pixels.resize(128 * 128);
    page.size = { 128, 128 };

    for (auto c : utf16)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) == 0)
        {
            WriteGlyphToPage(page, bitmap);
        }
    }



    for (auto c : utf16)
    {
        if(characters.find(c) == characters.end())
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                return -1;
                
            // generate texture
            auto texHandle = resourceHolder.create<Texture2D, 1>();
            GLuint texture = texHandle[0];
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap.width, bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);

            //std::string fName = "char_" + std::to_string(cnt) + ".png";
            // cnt++;

            // uncomment to output into files and console
            // stbi_write_png(fName.c_str(), bitmap.width, bitmap.rows, 1, bitmap.buffer, 0);
            // printf("Width: %u, Height: %u\n", bitmap.width, bitmap.rows);

            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use

            Character character =
            {
                glm::ivec2(bitmap.width, bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>(face->glyph->advance.x),
                texture
            };

            characters[c] = character;

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ftLibrary);

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
        glUseProgram(shader);
    
        glm::vec3 color(0.5, 0.8f, 0.2f);

        if(auto u = glGetUniformLocation(shader, "textColor"); u != -1)
            glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);

        RenderText(characters, vao, vbo, utf16Text, 340.0f, 370.0f);

        glUseProgram(0);

        window.display();
    }

    return 0;
}