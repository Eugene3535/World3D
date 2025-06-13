#include <array>
#include <memory>

#include <glad/glad.h>
#include <cglm/call/cam.h>
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
    vec3 viewPoint = { 50, 0, 50 };
    vec3 pos = { 250, 0, 250 };
    m_camera->focusOn(viewPoint);
    m_camera->setPosition(pos);

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
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (!m_isRotationMode && !m_isMovementMode)
        {
            m_previousMouse = sf::Vector2f(sf::Mouse::getPosition());
            m_isRotationMode = true;
        }
    }
    else m_isRotationMode = false;

    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
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
        m_camera->rotateAroundTarget(deltaX * 0.1f, deltaY * 0.1f);
        m_previousMouse = m_currentMouse;
    }
    else if (m_isMovementMode)
    {
        m_camera->movePamoramic(-deltaX * 0.1f, deltaY * 0.1f);
        m_previousMouse = m_currentMouse;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        m_camera->move(OrbitCamera::Forward, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        m_camera->move(OrbitCamera::Left, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        m_camera->move(OrbitCamera::Backward, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        m_camera->move(OrbitCamera::Right, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
        m_camera->move(OrbitCamera::Up, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        m_camera->move(OrbitCamera::Down, 0.1f);

    mat4 modelView;
    mat4 projection;
    mat4 mvp;

    auto [width, height] = m_window.getSize();

    m_camera->getModelViewMatrix(modelView);
    glmc_perspective(glm_rad(45), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000, projection);
    glmc_mat4_mul(projection, modelView, mvp);
    
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