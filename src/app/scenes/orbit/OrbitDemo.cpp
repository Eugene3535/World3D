#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "resources/holder/GlResourceHolder.hpp"
#include "resources/shaders/ShaderProgram.hpp"
#include "camera/orbit/OrbitCamera.hpp"
#include "scenes/orbit/OrbitDemo.hpp"


OrbitDemo::OrbitDemo(sf::Window& window) noexcept:
    DemoScene(window),
    m_previousMouse(),
    m_currentMouse(),
    m_mouseScrollDelta(0),
    m_isRotationMode(false),
    m_isMovementMode(true)
{

}


OrbitDemo::~OrbitDemo()
{

}


bool OrbitDemo::init() noexcept
{
    auto& holder = *GlResourceHolder::getInstance();
    
    auto [width, height] = m_window.getSize();

    const auto bufferHandles  = holder.create<GlBuffer, 2>();
    const auto vertexArrays   = holder.create<VertexArrayObject, 1>();
    const auto textureHandles = holder.create<Texture, 1>();

    m_uniformBuffer = std::make_unique<GlBuffer>(bufferHandles[0], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(mat4));

    m_camera = std::make_unique<OrbitCamera>();
    vec3 minPoint = {};
    vec3 maxPoint = { 100, 0, 100 };
    m_camera->setup(minPoint, maxPoint);
    m_camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));

    m_texture = std::make_unique<Texture>(textureHandles[0]);

    if(!m_texture->loadFromFile(FileProvider::findPathToFile("grid.png"), true, true)) 
        return false;

    std::array<float, 20> vertices = 
    {
        0.f,   0.f, 0.f,   0.f, 0.f,
        100.f, 0.f, 0.f,   1.f, 0.f,
        100.f, 0.f, 100.f, 1.f, 1.f,
        0.f,   0.f, 100.f, 0.f, 1.f
    };

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    GlBuffer vbo(bufferHandles[1], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    m_vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    m_vao->addVertexBuffer(vbo, attributes);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("orbit.vert"), GL_VERTEX_SHADER))   return false;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("orbit.frag"), GL_FRAGMENT_SHADER)) return false;

    if(m_program = std::make_unique<ShaderProgram>(); !m_program->link(shaders))
        return false;

    glUseProgram(m_program->getHandle());
    glUniform1i(m_program->getUniformLocation("texture0"), 0);
    glUseProgram(0);

    return true;
}


void OrbitDemo::update(const sf::Time& dt) noexcept
{
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (!m_isRotationMode && !m_isMovementMode)
        {
            m_previousMouse = sf::Vector2f(sf::Mouse::getPosition());
            m_isRotationMode = true;
        }
    }
    else m_isRotationMode = false;

    if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        if (!m_isRotationMode && !m_isMovementMode)
        {
            m_previousMouse = sf::Vector2f(sf::Mouse::getPosition());
            m_isMovementMode = true;
        }
    }
    else m_isMovementMode = false;

    m_currentMouse = sf::Vector2f(sf::Mouse::getPosition());

    const auto deltaX = static_cast<float>(m_currentMouse.x - m_previousMouse.x);
    const auto deltaY = static_cast<float>(m_currentMouse.y - m_previousMouse.y);

    if (m_isRotationMode)
    {
        m_camera->rotateAzimuth(deltaX * 0.01f);
        m_camera->rotatePolar(deltaY * 0.01f);
        m_previousMouse = m_currentMouse;
    }
    else if (m_isMovementMode)
    {
        m_camera->moveHorizontal(-deltaX * 0.25f);
        m_camera->moveVertical(deltaY * 0.25f);
        m_previousMouse = m_currentMouse;
    }

    if(m_mouseScrollDelta != 0.f)
        m_camera->zoom(-m_mouseScrollDelta);

    mat4 mvp;
    m_camera->getModelViewProjectionMatrix(mvp);

    m_uniformBuffer->update(0, sizeof(mat4), 1, static_cast<const void*>(mvp));
}


void OrbitDemo::draw() noexcept
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program->getHandle());

    glBindTextureUnit(0, m_texture->handle);
    glBindVertexArray(m_vao->getHandle());

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
    glBindTextureUnit(0, 0);
    glUseProgram(0);
    
    glDisable(GL_DEPTH_TEST);
}


void OrbitDemo::processMouseScroll(float delta) noexcept
{
    m_mouseScrollDelta = delta;
}