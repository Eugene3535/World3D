#include <array>
#include <memory>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
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


bool LightDemo::init(GlResourceHolder& holder) noexcept
{
    auto [width, height] = m_window.getSize();

    const auto bufferHandles  = holder.create<GlBuffer, 3>();
    const auto vertexArrays   = holder.create<VertexArrayObject, 2>();
    const auto textureHandles = holder.create<Texture, 2>();

    m_uniformBuffer = std::make_unique<GlBuffer>(bufferHandles[0], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(glm::mat4));

    m_camera = std::make_unique<Camera3D>();
    m_camera->m_position = { 0.f, 2.f, 4.f };
    m_camera->m_target   = { 0.f, 2.f, 0.f };
    m_camera->m_up       = { 0.f, 1.f, 0.f };

    m_planeTexture = std::make_unique<Texture>(textureHandles[0]);
    m_cubeTexture = std::make_unique<Texture>(textureHandles[1]);

    if(!m_planeTexture->loadFromFile(FileProvider::findPathToFile("grid.png"), true, true))
        return false;

    if(!m_cubeTexture->loadFromFile(FileProvider::findPathToFile("block.png"), true, true)) 
        return false;

    std::array<float, 36> planeVertices = 
    {
        -50.f, 0.f, -50.f, 0.0f, 0.0f, 0.f, 1.f, 0.f,
         50.f, 0.f, -50.f, 1.0f, 0.0f, 0.f, 1.f, 0.f,
         50.f, 0.f,  50.f, 1.0f, 1.0f, 0.f, 1.f, 0.f,
        -50.f, 0.f,  50.f, 0.0f, 1.0f, 0.f, 1.f, 0.f
    };

    std::array<const VertexBufferLayout::Attribute, 3> planeAttributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2,
        VertexBufferLayout::Attribute::Float3
    };

    GlBuffer planeVbo(bufferHandles[1], GL_ARRAY_BUFFER);
    planeVbo.create(sizeof(float), planeVertices.size(), static_cast<const void*>(planeVertices.data()), GL_STATIC_DRAW);

    m_planeVao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    m_planeVao->addVertexBuffer(planeVbo, planeAttributes);

    std::array<float, 180> cubeVertices = 
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    std::array<const VertexBufferLayout::Attribute, 2> cubeAttributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    GlBuffer cubeVbo(bufferHandles[2], GL_ARRAY_BUFFER);
    cubeVbo.create(sizeof(float), cubeVertices.size(), static_cast<const void*>(cubeVertices.data()), GL_STATIC_DRAW);

    m_cubeVao = std::make_unique<VertexArrayObject>(vertexArrays[1]);
    m_cubeVao->addVertexBuffer(cubeVbo, cubeAttributes);

    std::array<Shader, 2> shaders;

    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("light_plane.vert"), GL_VERTEX_SHADER)) 
        return false;

    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("light_plane.frag"), GL_FRAGMENT_SHADER)) 
        return false;

    if(m_planeProgram = std::make_unique<ShaderProgram>(); !m_planeProgram->link(shaders))
        return false;

    glUseProgram(m_planeProgram->getHandle());
    if(int uniform = m_planeProgram->getUniformLocation("texture0"); uniform != -1)
        glUniform1i(uniform, 0);
    glUseProgram(0);

    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("light_cube.vert"), GL_VERTEX_SHADER)) 
        return false;

    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("light_cube.frag"), GL_FRAGMENT_SHADER)) 
        return false;

    if(m_cubeProgram = std::make_unique<ShaderProgram>(); !m_cubeProgram->link(shaders))
        return false;

    glUseProgram(m_cubeProgram->getHandle());
    if(int uniform = m_cubeProgram->getUniformLocation("texture0"); uniform != -1)
        glUniform1i(uniform, 0);
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

    m_camera->update(deltaX, deltaY, m_camera->m_mode, dt.asSeconds());
}


void LightDemo::draw() noexcept
{
    auto [width, height] = m_window.getSize();
    auto projection = m_camera->getProjectionMatrix((float)width / (float)height);
    auto modelView = m_camera->getViewMatrix();
    auto mvp = projection * modelView;
    m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(mvp)));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.33f, 0.33f, 0.33f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_planeProgram->getHandle());

    glm::vec3 light_color = { 1, 1, 1 };
    float ambient_factor = 0.1f;
  
    glm::vec4 target(m_camera->m_target, 1.f);
    glm::vec4 light_position = modelView * target;
    

    if(int uniform = m_planeProgram->getUniformLocation("model_view_matrix"); uniform != -1)
        glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(modelView));

    if(int uniform = m_planeProgram->getUniformLocation("light_position"); uniform != -1)
        glUniform3fv(uniform, 1, glm::value_ptr(light_position));

    if(int uniform = m_planeProgram->getUniformLocation("light_color"); uniform != -1)
        glUniform3fv(uniform, 1, glm::value_ptr(light_color));

    if(int uniform = m_planeProgram->getUniformLocation("ambient_factor"); uniform != -1)
        glUniform1f(uniform, ambient_factor);

    glBindTexture(GL_TEXTURE_2D, m_planeTexture->handle);
    glBindVertexArray(m_planeVao->getHandle());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    const glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num1))
    {
        m_camera->m_mode = Camera3D::Free;
        m_camera->m_up = worldUp; // Reset roll
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num2))
    {
        m_camera->m_mode = Camera3D::FirstPerson;
        m_camera->m_up = worldUp; // Reset roll
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num3))
    {
        m_camera->m_mode = Camera3D::ThirdPerson;
        m_camera->m_up = worldUp; // Reset roll
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num4))
    {
        m_camera->m_mode = Camera3D::Orbital;
        m_camera->m_up = worldUp; // Reset roll
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::P))
    {
        m_camera->m_mode = Camera3D::ThirdPerson;
        m_camera->m_position = { 0.f, 2.f, -100.f };
        m_camera->m_target   = { 0.f, 2.f, 0.f };
        m_camera->m_up       = { 0.f, 1.f, 0.f };

        m_camera->rotateYaw(glm::radians(-135.f), true);
        m_camera->rotatePitch(glm::radians(-45.f), true, true, false);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::I))
    {
        m_camera->m_mode = Camera3D::ThirdPerson;
        m_camera->m_position = { 0.f, 2.f, 10.f };
        m_camera->m_target   = { 0.f, 2.f, 0.f };
        m_camera->m_up       = { 0.f, 1.f, 0.f };

        m_camera->rotateYaw(glm::radians(-135.f), true);
        m_camera->rotatePitch(glm::radians(-45.f), true, true, false);
    }

    if(m_camera->m_mode == Camera3D::ThirdPerson)
    {
        glm::mat4 model = mvp * glm::translate(glm::identity<glm::mat4>(), m_camera->m_target);
        m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(model)));

        glUseProgram(m_cubeProgram->getHandle());
        glBindTexture(GL_TEXTURE_2D, m_cubeTexture->handle);
        glBindVertexArray(m_cubeVao->getHandle());
        glDrawArrays(GL_TRIANGLES, 0, 180);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

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