#include <array>

#include <glad/glad.h>

#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/FileProvider.hpp"
#include "files/StbImage.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

    m_texture0 = std::make_unique<Texture>(textureHandles[0]);
    m_texture1 = std::make_unique<Texture>(textureHandles[1]);
    m_texture2 = std::make_unique<Texture>(textureHandles[2]);
    m_texture3 = std::make_unique<Texture>(textureHandles[3]);
    m_texture4 = std::make_unique<Texture>(textureHandles[4]);

    m_texture0->loadFromImage(*m_mapMask, false, true);
    m_texture1->loadFromImage(imgSand, true, true);
    m_texture2->loadFromImage(imgSpace, true, true);
    m_texture3->loadFromImage(imgStone, true, true);
    m_texture4->loadFromImage(imgRock, true, true);

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

    m_vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    m_vao->addVertexBuffer(vbo, attributes);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("dune.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("dune.frag"), GL_FRAGMENT_SHADER)) return -1;

    m_program = std::make_unique<ShaderProgram>();
    if(!m_program->link(shaders)) return -1;

    glUseProgram(m_program->getHandle());
    glUniform1i(m_program->getUniformLocation("texMap"), 0);
    glUniform1i(m_program->getUniformLocation("texSand"), 1);
    glUniform1i(m_program->getUniformLocation("texSpice"), 2);
    glUniform1i(m_program->getUniformLocation("texStone"), 3);
    glUniform1i(m_program->getUniformLocation("texRock"), 4);
    glUseProgram(0);

    m_uniformBuffer = std::make_unique<GlBuffer>(buffers[1], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(glm::mat4));

    m_camera = std::make_unique<OrthogonalCamera>();
    m_camera->setupProjectionMatrix(width, height);

    m_isLoaded = true;

    return m_isLoaded;
}


void DuneDemo::update(const sf::Time& dt) noexcept
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        m_camera->move(0.0f, 3.0f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        m_camera->move(3.0f, 0.0f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        m_camera->move(0.0f, -3.0f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        m_camera->move(-3.0f, 0.0f);

    m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(m_camera->getModelViewProjectionMatrix())));
}


void DuneDemo::draw() noexcept
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program->getHandle());

    glBindTextureUnit(0, m_texture0->handle);
    glBindTextureUnit(1, m_texture1->handle);
    glBindTextureUnit(2, m_texture2->handle);
    glBindTextureUnit(3, m_texture3->handle);
    glBindTextureUnit(4, m_texture4->handle);

    glBindVertexArray(m_vao->getHandle());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glBindTextureUnit(0, 0);
    glBindTextureUnit(1, 0);
    glBindTextureUnit(2, 0);
    glBindTextureUnit(3, 0);
    glBindTextureUnit(4, 0);

    glUseProgram(0);
}