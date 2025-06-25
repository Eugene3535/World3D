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
#include "camera/perspective/Camera3D.hpp"
#include "scenes/light/LightDemo.hpp"


LightDemo::LightDemo(sf::Window& window) noexcept:
    DemoScene(window),
    m_mouseScrollDelta(0)
{

}


LightDemo::~LightDemo()
{

}


bool LightDemo::init() noexcept
{
    auto& holder = *GlResourceHolder::getInstance();
    
    auto [width, height] = m_window.getSize();

    const auto bufferHandles  = holder.create<GlBuffer, 2>();
    const auto vertexArrays   = holder.create<VertexArrayObject, 1>();
    const auto textureHandles = holder.create<Texture, 1>();

    m_uniformBuffer = std::make_unique<GlBuffer>(bufferHandles[0], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(mat4));

    m_camera = std::make_unique<Camera3D>();
    vec3 position = { 0.f, 2.f, 4.f };  // Camera position
    vec3 target   = { 0.f, 2.f, 0.f };  // Camera looking at point
    vec3 up       = { 0.f, 1.f, 0.0f }; // Camera up vector (rotation towards target)
    glm_vec3_copy(position, m_camera->m_position);
    glm_vec3_copy(target, m_camera->m_target);
    glm_vec3_copy(up, m_camera->m_up);

    m_texture = std::make_unique<Texture>(textureHandles[0]);

    if(!m_texture->loadFromFile(FileProvider::findPathToFile("grid.png"), true, true)) 
        return false;

    std::array<float, 20> planeVertices = 
    {
        -50.f, 0.f,  -50.f, 0.0f, 0.0f,
         50.f,  0.f, -50.f, 1.0f, 0.0f,
         50.f,  0.f,  50.f, 1.0f, 1.0f,
        -50.f, 0.f,   50.f, 0.0f, 1.0f
    };

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    GlBuffer vbo(bufferHandles[1], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), planeVertices.size(), static_cast<const void*>(planeVertices.data()), GL_STATIC_DRAW);

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


void LightDemo::update(const sf::Time& dt) noexcept
{
    float deltaX = 0;
    float deltaY = 0;

    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        deltaX = m_mouseMovementDelta.x;
        deltaY = m_mouseMovementDelta.y;
    }

    auto [width, height] = m_window.getSize();

    m_camera->update(deltaX, deltaY, m_camera->m_mode, dt.asSeconds());

    mat4 projection, modelView, mvp;
    m_camera->getProjectionMatrix(projection, (float)width / (float)height);
    m_camera->getViewMatrix(modelView);
    glmc_mat4_mul(projection, modelView, mvp);

    m_uniformBuffer->update(0, sizeof(mat4), 1, static_cast<const void*>(mvp));
}


void LightDemo::draw() noexcept
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


void LightDemo::processMouseScroll(float delta) noexcept
{
    m_mouseScrollDelta = delta;
}


void LightDemo::processMouseMovement(float dx, float dy) noexcept
{
    m_mouseMovementDelta = { dx, dy };
}