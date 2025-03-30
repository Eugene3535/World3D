#include <array>
#include <memory>
#include <unordered_map>
#include <codecvt>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "files/Image.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"


// Holds all state information relevant to a character as loaded using FreeType
struct Character 
{
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Horizontal offset to advance to next glyph
    GLuint TextureID;   // ID handle of the glyph texture
};

using Characters = std::unordered_map<wchar_t, Character>;

static void RenderText(Characters& characters, VertexArrayObject& vao, GlBuffer& vbo, const std::wstring& text, float x, float y, float scale) noexcept
{
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao.getHandle());

    // iterate through all characters
    for (auto c : text)
    {
        const Character& ch = characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

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
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
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

    auto [width, height] = window.getSize();

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
    orthoCamera->setupProjectionMatrix(width, height);

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
    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::string utf8String = "Вау гречка ёЁ юЮ first commit 1234";
    std::wstring text = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8String);

    std::vector<GLuint> textures;
    Characters characters;

    for (auto c : text)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            return -1;
            
        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        textures.push_back(texture);

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use

        Character character =
        {
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x),
            texture
        };

        characters[c] = character;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ftLibrary);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::Resized)
            {
                width = event.size.width;
                height = event.size.height;
                orthoCamera->setupProjectionMatrix(width, height);
                glViewport(0, 0, width, height);
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

        RenderText(characters, vao, vbo, text, 340.0f, 370.0f, 1.0f);

        glUseProgram(0);

        window.display();
    }

    glDeleteTextures(static_cast<GLsizei>(textures.size()), textures.data());

    return 0;
}