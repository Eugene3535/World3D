#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/Image.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "data/AppData.hpp"
#include "tilemap/TileMap.hpp"


int platformer_demo(sf::Window& window, AppData& appData)
{
    auto [width, height] = window.getSize();

    std::array<uint32_t, 1> buffer = appData.resourceHolder.create<GlBuffer, 1>();

    GlBuffer uniformBuffer(buffer[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = &appData.camera.orthogonal;
    camera->setupProjectionMatrix(width, height);

    std::array<Shader, 2> shaders;
    if (!shaders[0].loadFromFile("res/shaders/tilemap.vert", GL_VERTEX_SHADER)) return -1;
    if (!shaders[1].loadFromFile("res/shaders/tilemap.frag", GL_FRAGMENT_SHADER)) return -1;

    auto tilemapProgram = std::make_unique<ShaderProgram>();
    if (!tilemapProgram->link(shaders)) return -1;

    glUseProgram(tilemapProgram->getHandle().value());
    glUniform1i(tilemapProgram->getUniformLocation("texture0").value(), 0);
    glUseProgram(0);

    TileMap tilemap(appData.resourceHolder);

    if(!tilemap.loadFromFile(FileProvider::findPathToFile("Level-1.tmx"))) 
        return -1;

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

        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        tilemap.draw(tilemapProgram.get());

        window.display();
    }

    return 0;
}