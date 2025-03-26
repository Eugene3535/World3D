#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/System/String.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/Image.hpp"
#include "files/Font.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
#include "graphics/Text.hpp"


int font_demo(sf::Window& window) noexcept
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    auto [width, height] = window.getSize();

    GlResourceHolder resourceHolder;
    std::array<uint32_t, 1> buffer = resourceHolder.create<GlBuffer, 1>();

    GlBuffer uniformBuffer(buffer[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto orthoCamera = std::make_unique<OrthogonalCamera>();
    orthoCamera->setupProjectionMatrix(width, height);

//  Shaders
    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("circle.vert"), GL_VERTEX_SHADER))   return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("circle.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return -1;

    glUseProgram(program->getHandle().value());
    glUniform1i(program->getUniformLocation("circleSampler").value(), 0);

//  Font
    Font font(resourceHolder);

    if( ! font.loadFromFile(FileProvider::findPathToFile("AvanteNrBook.ttf")) )
        return -1;

    Text text;
    text.setFont(&font);

    const std::string rawText("Simple Dimple");
    text.setString(sf::String::fromUtf8(rawText.begin(), rawText.end()));

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
                orthoCamera->setupProjectionMatrix(width, height);
                glViewport(0, 0, width, height);
            }
        }

        text.setPosition(200, 200);
        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(orthoCamera->getModelViewProjectionMatrix() * text.getMatrix())));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, text.getFont()->getTexture(30)->getHandle());
        text.draw();
        glBindTexture(GL_TEXTURE_2D, 0);

        window.display();
    }

    return 0;
}