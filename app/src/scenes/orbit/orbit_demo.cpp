#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "camera/orbit/OrbitCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"


int orbit_demo(sf::Window& window) noexcept
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    auto [width, height] = window.getSize();

    auto resourceHolder = std::make_unique<GlResourceHolder>();
    const auto bufferHandles = resourceHolder->create<GlBuffer, 2>();
    const auto vertexArrays = resourceHolder->create<VertexArrayObject, 1>();
    const auto textureHandles = resourceHolder->create<Texture2D, 1>();

    GlBuffer uniformBuffer(bufferHandles[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = std::make_unique<OrbitCamera>();
    camera->setup({ 0, 0, 0 }, { 100, 0, 100 });
    camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));

    auto texGrid = std::make_unique<Texture2D>(textureHandles[0]);

    if(!texGrid->loadFromFile(FileProvider::findPathToFile("grid.png"), true, true)) 
        return -1;

    std::array<float, 20> vertices = 
    {
        0.0f,   0.0f, 0.0f,   0.0f, 0.0f,
        100.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        100.0f, 0.0f, 100.0f, 1.0f, 1.0f,
        0.0f,   0.0f, 100.0f, 0.0f, 1.0f
    };

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    GlBuffer vbo(bufferHandles[1], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    auto vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    vao->addVertexBuffer(vbo, attributes);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("orbit.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("orbit.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return -1;

    glUseProgram(program->getHandle().value());
    glUniform1i(program->getUniformLocation("texture0").value(), 0);
    glUseProgram(0);

    sf::Clock clock;

    float prevMousePosX, prevMousePosY;
    float curMousePosX, curMousePosY;
    bool rotatingCamera = false;
    bool movingCamera = false;

    while (window.isOpen())
    {
        sf::Event event;

        float mouseScrollDelta = 0;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::Resized)
            {
                width = event.size.width;
                height = event.size.height;
                camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));
                glViewport(0, 0, width, height);
            }

            if(event.type == sf::Event::MouseWheelScrolled)
            {
                mouseScrollDelta = event.mouseWheelScroll.delta;
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            window.close();

        if(mouseScrollDelta != 0.0f)
            camera->zoom(-mouseScrollDelta);
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (!rotatingCamera && !movingCamera)
            {
                sf::Vector2f mousePos(sf::Mouse::getPosition());
                rotatingCamera = true;
                prevMousePosX = mousePos.x;
                prevMousePosY = mousePos.y;
            }
        }
        else rotatingCamera = false;

        if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if (!rotatingCamera && !movingCamera)
            {
                sf::Vector2f mousePos(sf::Mouse::getPosition());
                movingCamera = true;
                prevMousePosX = mousePos.x;
                prevMousePosY = mousePos.y;
            }
        }
        else movingCamera = false;


        sf::Vector2f mousePos(sf::Mouse::getPosition());
        curMousePosX = mousePos.x;
        curMousePosY = mousePos.y;
        const auto deltaX = static_cast<float>(curMousePosX - prevMousePosX);
        const auto deltaY = static_cast<float>(curMousePosY - prevMousePosY);
    
        if (rotatingCamera)
        {
            camera->rotateAzimuth(deltaX * 0.01f);
            camera->rotatePolar(deltaY * 0.01f);
            prevMousePosX = curMousePosX;
            prevMousePosY = curMousePosY;
        }
        else if (movingCamera)
        {
            camera->moveHorizontal(-deltaX * 0.25f);
            camera->moveVertical(deltaY * 0.25f);
            prevMousePosX = curMousePosX;
            prevMousePosY = curMousePosY;
        }

		uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program->getHandle().value());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texGrid->getHandle());
        glBindVertexArray(vao->getHandle());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        window.display();
    }

    return 0;
}