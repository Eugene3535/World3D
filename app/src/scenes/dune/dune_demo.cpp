#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"

int dune_demo(sf::Window& window) noexcept
{
    auto [width, height] = window.getSize();

    StbImage imgMask;
    StbImage imgSand;
    StbImage imgSpace;
    StbImage imgRock;
    StbImage imgStone;

    if(!imgMask.loadFromFile(FileProvider::findPathToFile("mask.png")))   return -1;
    if(!imgSand.loadFromFile(FileProvider::findPathToFile("sand.jpg")))   return -1;
    if(!imgSpace.loadFromFile(FileProvider::findPathToFile("spice.jpg"))) return -1;
    if(!imgRock.loadFromFile(FileProvider::findPathToFile("rock01.jpg"))) return -1;
    if(!imgStone.loadFromFile(FileProvider::findPathToFile("stone.jpg"))) return -1;

    auto resourceHolder = std::make_unique<GlResourceHolder>();
    std::array<uint32_t, 5> textures = resourceHolder->create<Texture, 5>();

    auto texture0 = std::make_unique<Texture>(textures[0]);
    auto texture1 = std::make_unique<Texture>(textures[1]);
    auto texture2 = std::make_unique<Texture>(textures[2]);
    auto texture3 = std::make_unique<Texture>(textures[3]);
    auto texture4 = std::make_unique<Texture>(textures[4]);

    texture0->loadFromImage(imgMask, false, true);
    texture1->loadFromImage(imgSand, true, true);
    texture2->loadFromImage(imgSpace, true, true);
    texture3->loadFromImage(imgStone, true, true);
    texture4->loadFromImage(imgRock, true, true);

    std::array<float, 20> vertices =
    {
        0.0f,                      0.0f,                       0.0f, 0.0f, 0.0f,
        static_cast<float>(width), 0.0f,                       0.0f, 1.0f, 0.0f,
        static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f, 1.0f,
        0.0f,                      static_cast<float>(height), 0.0f, 0.0f, 1.0f
    };

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    std::array<uint32_t, 2> buffers = resourceHolder->create<GlBuffer, 2>();
    std::array<uint32_t, 1> vertexArrays = resourceHolder->create<VertexArrayObject, 1>();

    GlBuffer vbo(buffers[0], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    auto vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    vao->addVertexBuffer(vbo, attributes);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("dune.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("dune.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return -1;

    glUseProgram(program->getHandle().value());
    glUniform1i(program->getUniformLocation("texMap").value(), 0);
    glUniform1i(program->getUniformLocation("texSand").value(), 1);
    glUniform1i(program->getUniformLocation("texSpice").value(), 2);
    glUniform1i(program->getUniformLocation("texStone").value(), 3);
    glUniform1i(program->getUniformLocation("texRock").value(), 4);
    glUseProgram(0);

    GlBuffer uniformBuffer(buffers[1], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = std::make_unique<OrthogonalCamera>();
    camera->setupProjectionMatrix(width, height);

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
                camera->setupProjectionMatrix(width, height);
                glViewport(0, 0, width, height);
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            window.close();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            camera->move(0.0f, 3.0f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            camera->move(3.0f, 0.0f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            camera->move(0.0f, -3.0f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            camera->move(-3.0f, 0.0f);

        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program->getHandle().value());

        glBindTextureUnit(0, texture0->handle);
        glBindTextureUnit(1, texture1->handle);
        glBindTextureUnit(2, texture2->handle);
        glBindTextureUnit(3, texture3->handle);
        glBindTextureUnit(4, texture4->handle);

        glBindVertexArray(vao->getHandle());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glBindTextureUnit(0, 0);
        glBindTextureUnit(1, 0);
        glBindTextureUnit(2, 0);
        glBindTextureUnit(3, 0);
        glBindTextureUnit(4, 0);

        glUseProgram(0);

        window.display();
    }

    return 0;
}