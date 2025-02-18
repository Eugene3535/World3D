#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/Image.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/Orthogonal.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
#include "tilemap/TileMap.hpp"
#include "sprites/SpriteHolder.hpp"
#include "animation/Animator.hpp"


int platformer_demo(sf::Window& window)
{
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    auto [width, height] = window.getSize();

    auto resourceHolder = std::make_unique<GlResourceHolder>();
    const auto buffers = resourceHolder->create<GlBuffer, 2>();

    GlBuffer uniformBuffer(buffers[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = std::make_unique<Orthogonal>(); 
    camera->setupProjectionMatrix(width, height);

    std::array<Shader, 2> shaders;
    if (!shaders[0].loadFromFile(FileProvider::findPathToFile("tilemap.vert"), GL_VERTEX_SHADER)) return -1;
    if (!shaders[1].loadFromFile(FileProvider::findPathToFile("tilemap.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto tilemapProgram = std::make_unique<ShaderProgram>();
    if (!tilemapProgram->link(shaders)) return -1;

    glUseProgram(tilemapProgram->getHandle().value());
    glUniform1i(tilemapProgram->getUniformLocation("texture0").value(), 0);
    glUseProgram(0);

    TileMap tilemap(*resourceHolder);

    if(!tilemap.loadFromFile(FileProvider::findPathToFile("Level-1.tmx"))) 
        return -1;

    const auto textureHandles = resourceHolder->create<Texture2D, 1>();
    auto texture = std::make_unique<Texture2D>(textureHandles[0]);

    if(!texture->loadFromFile(FileProvider::findPathToFile("enemy.png"), false, false)) 
        return -1;

    SpriteHolder spriteHolder(buffers[1]);

    const auto vaoHandles = resourceHolder->create<VertexArrayObject, 1>();
    auto vao = std::make_unique<VertexArrayObject>(vaoHandles[0]);

    const std::array<VertexBufferLayout::Attribute, 1> spriteAttributes
    {
        VertexBufferLayout::Attribute::Float4
    };
    const VertexBufferLayout spriteLayout(spriteAttributes);
    vao->addVertexBuffer(spriteHolder.getVertexBuffer(), spriteLayout);

    spriteHolder.createLinearAnimaton("move", texture.get(), 4);
    auto sprites = spriteHolder.getSprites("move");

    Animator animator;
    animator.addAnimation("move", sprites);
    animator.setLoop(true);
    animator.setRate(1);
    animator.play();

    sf::Clock clock;

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

        auto dt = clock.restart().asSeconds();
        animator.update(dt);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            window.close();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            camera->move(0.0f, 10.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            camera->move(10.0f, 0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            camera->move(0.0f, -10.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            camera->move(-10.0f, 0.0f);

        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glClear(GL_COLOR_BUFFER_BIT);
        tilemap.draw(tilemapProgram.get());

        glUseProgram(tilemapProgram->getHandle().value());
        glBindVertexArray(vao->getHandle());

        animator.draw();

        glBindVertexArray(0);
        glUseProgram(0);

        window.display();
    }

    return 0;
}