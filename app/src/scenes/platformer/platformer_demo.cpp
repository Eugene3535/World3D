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
#include "scenes/platformer/tilemap/TileMap.hpp"
#include "scenes/platformer/sprites/SpriteHolder.hpp"
#include "scenes/platformer/entities/Goomba.hpp"
#include "scenes/platformer/entities/Player.hpp"


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

//  Textures
    const auto textureHandles = resourceHolder->create<Texture2D, 2>();

    auto texGoomba = std::make_unique<Texture2D>(textureHandles[0]);
    auto texMegaman = std::make_unique<Texture2D>(textureHandles[1]);

    if(!texGoomba->loadFromFile(FileProvider::findPathToFile("enemy.png"), false, false)) return -1;
    if(!texMegaman->loadFromFile(FileProvider::findPathToFile("megaman.png"), false, false)) return -1;

//  Sprite animation
    SpriteHolder spriteHolder(buffers[1]);

    const auto vaoHandles = resourceHolder->create<VertexArrayObject, 1>();
    auto vao = std::make_unique<VertexArrayObject>(vaoHandles[0]);

    const std::array<VertexBufferLayout::Attribute, 1> spriteAttributes
    {
        VertexBufferLayout::Attribute::Float4
    };
    const VertexBufferLayout spriteLayout(spriteAttributes);
    vao->addVertexBuffer(spriteHolder.getVertexBuffer(), spriteLayout);

//  Goomba
    {
        std::array<glm::ivec4, 2> goombaWalkFrames = 
        {
            glm::ivec4(0, 0, 16, 16),
            glm::ivec4(16, 0, 16, 16)
        };
        spriteHolder.createCustomAnimaton(GOOMBA_WALK, texGoomba.get(), goombaWalkFrames);
        spriteHolder.createSingleAnimation(GOOMBA_DEAD, texGoomba.get(), glm::ivec4(48, 0, 16, 16));
    }

    Animator goomba;
    {
        auto goombaWalkAnim = spriteHolder.getSprites(GOOMBA_WALK);
        auto goombaDeadAnim = spriteHolder.getSprites(GOOMBA_DEAD);
        goomba.addAnimation(GOOMBA_WALK, goombaWalkAnim);
        goomba.addAnimation(GOOMBA_DEAD, goombaDeadAnim);
        goomba.setLoop(true);
        goomba.setRate(4);
        goomba.play();
    }

//  Megaman
    //spriteHolder.loadSpriteSheet(FileProvider::findPathToFile("anim_megaman.xml"), texMegaman.get());

    auto enemyObjects = tilemap.getObjectsByName("enemy");

    std::vector<std::unique_ptr<Entity>> entities;

    for (const auto object : enemyObjects)
        auto entity = entities.emplace_back(std::make_unique<Goomba>(goomba, object->bounds.left, object->bounds.top)).get();
        
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

        for(auto& entity : entities)
            entity->update(dt);

        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glUseProgram(tilemapProgram->getHandle().value());

        glClear(GL_COLOR_BUFFER_BIT);
        tilemap.draw();

        glBindVertexArray(vao->getHandle());

        for(auto& entity : entities)
        {
            uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix() * entity->getMatrix())));
            entity->anim.draw();
        }
            
        glBindVertexArray(0);

        glUseProgram(0);

        window.display();
    }

    return 0;
}