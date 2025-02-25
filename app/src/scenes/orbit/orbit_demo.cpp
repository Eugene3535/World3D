#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/Image.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/perspective/Perspective.hpp"
#include "opengl/holder/GlResourceHolder.hpp"


int orbit_demo(sf::Window& window) noexcept
{
    window.setMouseCursorVisible(false);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    auto [width, height] = window.getSize();

    auto resourceHolder = std::make_unique<GlResourceHolder>();;
    const auto bufferHandles = resourceHolder->create<GlBuffer, 2>();
    const auto vertexArrays = resourceHolder->create<VertexArrayObject, 1>();
    const auto textureHandles = resourceHolder->create<Texture2D, 1>();

    GlBuffer uniformBuffer(bufferHandles[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = std::make_unique<Perspective>();
    camera->setupProjectionMatrix(45, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);

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
    VertexBufferLayout layout(attributes);

    GlBuffer vbo(bufferHandles[1], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    auto vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    vao->addVertexBuffer(vbo, layout);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("orbit.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("orbit.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return -1;

    glUseProgram(program->getHandle().value());
    glUniform1i(program->getUniformLocation("texture0").value(), 0);
    glUseProgram(0);

    glm::vec3 pos = {0, 50, 0};
    camera->setPosition(pos);

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
                camera->setupProjectionMatrix(45, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
                glViewport(0, 0, width, height);
            }

            if(event.type == sf::Event::MouseWheelScrolled)
            {
                mouseScrollDelta = event.mouseWheelScroll.delta;
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            window.close();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
        {
            camera->revertToOrigin(50);
        }

        if(mouseScrollDelta != 0.0f)
        {
            camera->zoom(mouseScrollDelta);
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            camera->move(0.01f);
        }

        const auto [xpos, ypos] = sf::Mouse::getPosition();
		auto [xt, yt] = window.getPosition();

		xt += width >> 1;
		yt += height >> 1;

		camera->rotateX((xt - xpos) * 0.125f);
		camera->rotateY((yt - ypos) * 0.125f);

        sf::Mouse::setPosition({xt, yt});

		camera->apply();
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