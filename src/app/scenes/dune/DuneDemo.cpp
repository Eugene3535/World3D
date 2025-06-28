#include <array>

#include <glad/glad.h>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "files/FileProvider.hpp"
#include "files/StbImage.hpp"
#include "resources/holder/GlResourceHolder.hpp"
#include "resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "scenes/dune/DuneDemo.hpp"


DuneDemo::DuneDemo(sf::Window& window) noexcept:
    DemoScene(window)
{

}


DuneDemo::~DuneDemo()
{

}


bool DuneDemo::init(GlResourceHolder& holder) noexcept
{
    StbImage imgSand;
    StbImage imgSpace;
    StbImage imgRock;
    StbImage imgStone;

    m_mapMask = std::make_unique<StbImage>();
    if(!m_mapMask->loadFromFile(FileProvider::findPathToFile("mask.png"))) return false;
    if(!imgSand.loadFromFile(FileProvider::findPathToFile("sand.jpg")))   return false;
    if(!imgSpace.loadFromFile(FileProvider::findPathToFile("spice.jpg"))) return false;
    if(!imgRock.loadFromFile(FileProvider::findPathToFile("rock01.jpg"))) return false;
    if(!imgStone.loadFromFile(FileProvider::findPathToFile("stone.jpg"))) return false;

    auto textureHandles = holder.create<Texture, 5>();
    m_textures.resize(textureHandles.size());

    for(size_t i = 0; i < textureHandles.size(); ++i)
        m_textures[i].handle = textureHandles[i];  

    m_textures[0].loadFromImage(*m_mapMask, false, true);
    m_textures[1].loadFromImage(imgSand, true, true);
    m_textures[2].loadFromImage(imgSpace, true, true);
    m_textures[3].loadFromImage(imgStone, true, true);
    m_textures[4].loadFromImage(imgRock, true, true);

    auto [width, height] = m_window.getSize();

    std::array<float, 20> vertices =
    {
        0.f,                       0.f,                        0.f, 0.f, 0.f,
        static_cast<float>(width), 0.f,                        0.f, 1.f, 0.f,
        static_cast<float>(width), static_cast<float>(height), 0.f, 1.f, 1.f,
        0.f,                       static_cast<float>(height), 0.f, 0.f, 1.f
    };

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    std::array<uint32_t, 2> buffers = holder.create<GlBuffer, 2>();
    std::array<uint32_t, 1> vertexArrays = holder.create<VertexArrayObject, 1>();

    GlBuffer vbo(buffers[0], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    VertexArrayObject vao(vertexArrays[0]);
    vao.addVertexBuffer(vbo, attributes);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("dune.vert"), GL_VERTEX_SHADER))   return false;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("dune.frag"), GL_FRAGMENT_SHADER)) return false;

    m_program = std::make_unique<ShaderProgram>();

    if(!m_program->link(shaders)) 
        return false;

    glUseProgram(m_program->getHandle());
    glUniform1i(m_program->getUniformLocation("texMap"), 0);
    glUniform1i(m_program->getUniformLocation("texSand"), 1);
    glUniform1i(m_program->getUniformLocation("texSpice"), 2);
    glUniform1i(m_program->getUniformLocation("texStone"), 3);
    glUniform1i(m_program->getUniformLocation("texRock"), 4);
    glUseProgram(0);

    m_uniformBuffer = std::make_unique<GlBuffer>(buffers[1], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(mat4));

    m_camera = std::make_unique<OrthogonalCamera>();
    m_camera->setupProjectionMatrix(width, height);

    m_mesh.vao = vao.getHandle();
    m_mesh.textures = m_textures;
    m_mesh.vertexCount = vbo.getCount();

    return true;
}


void DuneDemo::update(const sf::Time& dt) noexcept
{
    vec2 offset = {};
    bool is_key_pressed = false;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        offset[1] = 3;
        is_key_pressed = true;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        offset[0] = 3;
        is_key_pressed = true;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        offset[1] = -3;
        is_key_pressed = true;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        offset[0] = -3;
        is_key_pressed = true;
    }

    if(is_key_pressed)
        m_camera->move(offset);

    mat4 mvp;
    m_camera->getModelViewProjectionMatrix(mvp);

    m_uniformBuffer->update(0, sizeof(mat4), 1, static_cast<const void*>(mvp));
}


void DuneDemo::draw() noexcept
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program->getHandle());

    for(size_t i = 0; i < m_mesh.textures.size(); ++i)
        glBindTextureUnit(i, m_mesh.textures[i].handle);

    glBindVertexArray(m_mesh.vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    for(size_t i = 0; i < m_mesh.textures.size(); ++i)
        glBindTextureUnit(i, 0);

    glUseProgram(0);
}