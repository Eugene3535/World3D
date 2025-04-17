#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"


int path_demo(sf::Window& window)
{
    window.setMouseCursorVisible(false);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    StbImage imgSnow;     imgSnow.loadFromFile(FileProvider::findPathToFile("snow.png"));
    StbImage imgPavement; imgPavement.loadFromFile(FileProvider::findPathToFile("pavement.jpg"));
    StbImage imgPath;     imgPath.loadFromFile(FileProvider::findPathToFile("test.png"));

    auto resourceHolder = std::make_unique<GlResourceHolder>();
    auto textures = resourceHolder->create<Texture, 3>();

    Texture texSnow;
    Texture texPavement;
    Texture texPath;

    texSnow.handle = textures[0];
    texPavement.handle = textures[1];
    texPath.handle = textures[2];

    texSnow.loadFromImage(imgSnow, true, true);
    texPavement.loadFromImage(imgPavement, true, true);
    texPath.loadFromImage(imgPath, true, true);

    float mapWidth  = static_cast<float>(imgPath.width);
    float mapHeight = static_cast<float>(imgPath.height);

    std::array<float, 20> vertices = 
    {
        0.0f,     0.0f, 0.0f,      0.0f,  0.0f,
        mapWidth, 0.0f, 0.0f,      10.0f, 0.0f,
        mapWidth, 0.0f, mapHeight, 10.0f, 10.0f,
        0.0f,     0.0f, mapHeight, 0.0f,  10.0f
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
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("ground.vert"), GL_VERTEX_SHADER))   return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("ground.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return -1;

    glUseProgram(program->getHandle().value());

    if(GLint uniformLoc = program->getUniformLocation("snowSampler"); uniformLoc != -1)
        glUniform1i(uniformLoc, 0);

    if(GLint uniformLoc = program->getUniformLocation("snowSampler"); uniformLoc != -1)
        glUniform1i(uniformLoc, 1);

    if(GLint uniformLoc = program->getUniformLocation("pathSampler"); uniformLoc != -1)
        glUniform1i(uniformLoc, 2);

    glUseProgram(0);

    GlBuffer uniformBuffer(buffers[1], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto [width, height] = window.getSize();
    auto camera = std::make_unique<PerspectiveCamera>();
    camera->setDrawDistance(500);
    camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));
    camera->setPosition(3, 3, 3);

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
                camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));
                glViewport(0, 0, width, height);
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            window.close();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            camera->processKeyboard(PerspectiveCamera::Forward, 1);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            camera->processKeyboard(PerspectiveCamera::Left, 1);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            camera->processKeyboard(PerspectiveCamera::Backward, 1);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            camera->processKeyboard(PerspectiveCamera::Right, 1);

        auto playerPos = camera->getPosition();
        playerPos.y = 30;
        camera->setPosition(playerPos);

        const auto [xpos, ypos] = sf::Mouse::getPosition();
		auto [xt, yt] = window.getPosition();

		xt += width >> 1;
		yt += height >> 1;

		camera->processMouseMovement((xt - xpos) * 0.125f, (yt - ypos) * 0.125f);

        sf::Mouse::setPosition({xt, yt});

		uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUseProgram(program->getHandle().value());

        glBindTextureUnit(0, texSnow.handle);
        glBindTextureUnit(1, texPavement.handle);
        glBindTextureUnit(2, texPath.handle);

        glBindVertexArray(vao->getHandle());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glBindTextureUnit(0, 0);
        glBindTextureUnit(1, 0);
        glBindTextureUnit(2, 0);

        glUseProgram(0);

        window.display();
    }

    return 0;
}