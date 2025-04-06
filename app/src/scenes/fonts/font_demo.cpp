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
    Row(uint32_t rowTop, uint32_t rowHeight) noexcept : width(0), top(rowTop), height(rowHeight) {}

    uint32_t width;  // Current width of the row
    uint32_t top;    // Y position of the row into the texture
    uint32_t height; // Height of the row
};

using GlyphTable = std::unordered_map<wchar_t, Glyph>;

struct Page
{
    Page() noexcept: nextRow(0), size(0, 0), texture(0) {}

    GlyphTable           glyphs;  // Table mapping code points to their corresponding glyph
    std::vector<Row>     rows;    // List containing the position of all the existing rows
    uint32_t             nextRow; // Y position of the next new row in the texture
    std::vector<uint8_t> image;   // Image handle containing the pixels of the glyphs
    glm::ivec2           size;    // Size of page in pixels
    GLuint texture;
};


glm::ivec4 findGlyphRect(Page& page, uint32_t width, uint32_t height) noexcept
{
    Row* row = nullptr;
    float bestRatio = 0;

    for (auto it = page.rows.begin(); it != page.rows.end() && !row; ++it)
    {
        float ratio = static_cast<float>(height) / static_cast<float>(it->height);

        if ((ratio < 0.7f) || (ratio > 1.f))
            continue;

        if (width > page.size.x - it->width)
            continue;

        if (ratio < bestRatio)
            continue;

        row = &*it;
        bestRatio = ratio;
    }

    if (!row) 
    {
        uint32_t rowHeight = height + height / 10;

        if((page.nextRow + rowHeight >= page.size.y) || (width >= page.size.x)) // Make the image 2 times bigger
        {
            uint32_t imageWidth     = page.size.x;
            uint32_t imageHeight    = page.size.y;
            uint32_t newImageWidth  = imageWidth << 1;
            uint32_t newImageHeight = imageHeight << 1;

            std::vector<uint8_t> newImage(newImageWidth * newImageHeight);

            const uint8_t* oldPixels = page.image.data();
            uint8_t* newPixels = newImage.data();

            for (uint32_t i = 0; i < imageHeight; ++i)
            {
                memcpy(newPixels, oldPixels, imageWidth);
                oldPixels += imageWidth;
                newPixels += newImageWidth;
            }

            page.image.swap(newImage);
            page.size = { newImageWidth, newImageHeight };
        }

        page.rows.emplace_back(page.nextRow, rowHeight);
        page.nextRow += rowHeight;
        row = &page.rows.back();
    }

    glm::ivec4 rect(row->width, row->top, width, height);
    row->width += width;

    return rect;
}



void WriteGlyphToPage(wchar_t wc, Page& page, const FT_Face face) noexcept
{
    const auto& bitmap = face->glyph->bitmap;

    if(auto it = page.glyphs.find(wc); it == page.glyphs.end())
    {
        Glyph& glyph = page.glyphs[wc];

        uint32_t width  = bitmap.width;
        uint32_t height = bitmap.rows;
    
        const uint32_t rows = bitmap.rows;
        const uint32_t columns = bitmap.width;
        const uint32_t padding = 2;
    
        width  += padding << 1;
        height += padding << 1;
    
        auto textureRect = findGlyphRect(page, width, height);
        glyph.textureRect = textureRect;
        glyph.textureRect.x += static_cast<int>(padding);
        glyph.textureRect.y += static_cast<int>(padding);
        glyph.textureRect.z -= static_cast<int>(padding << 1);
        glyph.textureRect.w -= static_cast<int>(padding << 1);
    
        uint32_t stride = page.size.x;
        const uint8_t* srcPixels = bitmap.buffer;
        uint8_t*       dstPixels = page.image.data() + (textureRect.y * stride + textureRect.x);
    
        for (uint32_t i = 0; i < rows; ++i)
        {
            memcpy(dstPixels, srcPixels, columns);
            srcPixels += columns;
            dstPixels += stride;
        }

        glyph.bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top };
        glyph.size    = { bitmap.width, bitmap.rows };
        glyph.advance = face->glyph->advance.x;
    }
}



static void renderText(Page& page, VertexArrayObject& vao, GlBuffer& vbo, const std::wstring& text, float x, float y) noexcept
{
    auto& glyphs = page.glyphs;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, page.texture);
    glBindVertexArray(vao.getHandle());

    // iterate through all characters
    for (auto wc : text)
    {
        const Glyph& glyph = glyphs[wc];

        float xpos = x + glyph.bearing.x;
        float ypos = y - (glyph.size.y - glyph.bearing.y);

        float w = glyph.size.x;
        float h = glyph.size.y;

        glm::vec4 rect = glyph.textureRect;

        rect.x -= 2;
        rect.y -= 2;
        // rect.z += 2;
        // rect.w += 2;

        float left   = rect.x / page.size.x;
        float top    = rect.y / page.size.y;
        float right  = (rect.x + rect.z) / page.size.x;
        float bottom = (rect.y + rect.w) / page.size.y;

        // update VBO for each character
        float vertices[16] = 
        {
            xpos,     ypos,     left, bottom,
            xpos + w, ypos,     right, bottom,
            xpos + w, ypos + h, right, top,
            xpos, ypos + h,     left, top
        };

        // update content of VBO memory
        vbo.update(0, sizeof(glm::vec4), 4, vertices);

        // render quad
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (glyph.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    FT_Library ftLibrary;
    FT_Face face;

    if (FT_Init_FreeType(&ftLibrary))
        return -1;

    if (FT_New_Face(ftLibrary, FileProvider::findPathToFile("AvanteNrBook.ttf").generic_string().c_str(), 0, &face))
        return -1;

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 30);

    const char utf8[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя0123456789!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~ ";
    const std::wstring utf16(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8));

    const char utf8Text[] = "Вау гречка ёЁ-юЮ-ыЫ first commit 1234 ()*<>q";
    std::wstring utf16Text(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8Text));

    Page page;
    page.image.resize(128 * 128);
    page.size = { 128, 128 };

    for(auto wc : utf16)
        if(FT_Load_Char(face, wc, FT_LOAD_RENDER) == 0)
            WriteGlyphToPage(wc, page, face);

    stbi_write_png("test.png", page.size.x, page.size.y, 1, page.image.data(), 0);

//  disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto texHandle = resourceHolder.create<Texture2D, 1>();
    GLuint texture = texHandle[0];
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, page.size.x, page.size.y, 0, GL_RED, GL_UNSIGNED_BYTE, page.image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    page.texture = texture;
    
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

        renderText(page, vao, vbo, utf16Text, 340.0f, 370.0f);

        glUseProgram(0);

        window.display();
    }

    return 0;
}