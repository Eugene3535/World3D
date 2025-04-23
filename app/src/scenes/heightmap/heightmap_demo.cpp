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
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"


int heightmap_demo(sf::Window& window) noexcept
{
    glEnable(GL_DEPTH_TEST);

    auto [width, height] = window.getSize();

    window.setMouseCursorVisible(false);

    GlResourceHolder resourceHolder;
    std::array<uint32_t, 1> buffer = resourceHolder.create<GlBuffer, 1>();

    GlBuffer uniformBuffer(buffer[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = std::make_unique<PerspectiveCamera>();
    camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));
    camera->setDrawDistance(100);
    camera ->setPosition(30, 3, 30);

    auto orthoCamera = std::make_unique<OrthogonalCamera>();
    orthoCamera->setupProjectionMatrix(width, height);

    StbImage imageMap;        if(!imageMap.loadFromFile(FileProvider::findPathToFile("heightmap.png")))            return -1;
    StbImage imgCrackedEarth; if(!imgCrackedEarth.loadFromFile(FileProvider::findPathToFile("cracked_earth.jpg"))) return -1;
    StbImage imgRock;         if(!imgRock.loadFromFile(FileProvider::findPathToFile("rock.jpg")))                  return -1;
    StbImage imgGrass;        if(!imgGrass.loadFromFile(FileProvider::findPathToFile("grass.jpg")) )               return -1;
    StbImage imgClover;       if(!imgClover.loadFromFile(FileProvider::findPathToFile("clover.png")))              return -1;
    StbImage imageCircleOff;  if(!imageCircleOff.loadFromFile(FileProvider::findPathToFile("circle_off.png")))     return -1;
    StbImage imageCircleOn;   if(!imageCircleOn.loadFromFile(FileProvider::findPathToFile("circle_on.png")))       return -1;

    const auto textures = resourceHolder.create<Texture, 6>();
    
    Texture texCrackedEarth = { textures[0], GL_TEXTURE_2D, 0, 0, 0, false, false };
    Texture texRock         = { textures[1], GL_TEXTURE_2D, 0, 0, 0, false, false };
    Texture texGrass        = { textures[2], GL_TEXTURE_2D, 0, 0, 0, false, false };
    Texture texClover       = { textures[3], GL_TEXTURE_2D, 0, 0, 0, false, false };
    Texture texCircleOff    = { textures[4], GL_TEXTURE_2D, 0, 0, 0, false, false };
    Texture texCircleOn     = { textures[5], GL_TEXTURE_2D, 0, 0, 0, false, false };

    if(!texCrackedEarth.loadFromImage(imgCrackedEarth, true, true)) return -1;
    if(!texRock.loadFromImage(imgRock, true, true)) return -1;
    if(!texGrass.loadFromImage(imgGrass, true, true)) return -1;
    if(!texClover.loadFromImage(imgClover, true, true)) return -1;
    if(!texCircleOff.loadFromImage(imageCircleOff, false, false)) return -1;
    if(!texCircleOn.loadFromImage(imageCircleOn, false, false)) return -1;

    const uint8_t* pixels = imageMap.pixels.get();
    const uint32_t mapDepth = imageMap.width;
    const uint32_t mapWidth = imageMap.height;

    std::vector<float> heightmap;
    heightmap.resize(mapDepth * mapWidth);
    std::vector<float> vertices(mapDepth * mapWidth * 5);
    std::vector<uint32_t> indices;
    indices.reserve(static_cast<size_t>(mapDepth * mapWidth * 2));

    size_t index = 0;
    size_t stride = 0;

    for (size_t z = 0; z < mapDepth; ++z)
    {
        for (size_t x = 0; x < mapWidth; ++x)
        {
            const uint8_t* pixel = pixels + ((z * mapWidth + x) * imageMap.bytePerPixel);
            int32_t y = static_cast<int32_t>(pixel[0]);
            float Y = y * 0.03f;

            float* vertex = vertices.data() + stride;

            vertex[0] = x;
            vertex[1] = Y;
            vertex[2] = z;
            vertex[3] = x * 0.3f;
            vertex[4] = z * 0.3f;

            heightmap[index] = Y;

            ++index;
            stride += 5;
        }
    }

    for(uint32_t i = 0; i < mapDepth - 1; ++i) // 19 800
    {
        for(uint32_t j = 0; j < mapWidth; ++j)
        {
            indices.push_back(mapWidth * i + j);
            indices.push_back(mapWidth * (i + 1) + j);       
        }
    }

//  Allocate buffers
    const auto buffers = resourceHolder.create<GlBuffer, 3>();
    const auto vertexArrays = resourceHolder.create<VertexArrayObject, 2>();

//  Heightmap
    const std::array<VertexBufferLayout::Attribute, 2> heightmapAttributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    GlBuffer heightmapVbo(buffers[0], GL_ARRAY_BUFFER);
    GlBuffer heightmapEbo(buffers[1], GL_ELEMENT_ARRAY_BUFFER);

//  Just to give you an example, you can pass a pointer to the data right away, or you can fill the buffer later on
    heightmapVbo.create(sizeof(float), 0, nullptr, GL_STATIC_DRAW);
    heightmapVbo.update(0, sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()));
    heightmapEbo.create(sizeof(uint32_t), indices.size(), static_cast<const void*>(indices.data()), GL_STATIC_DRAW);

    auto heightmapVao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    heightmapVao->addVertexBuffer(heightmapVbo, heightmapAttributes);
    heightmapVao->setElementBuffer(heightmapEbo);

//  Circle
    vertices.clear();
    vertices =
    {
        0.0f,   0.0f,   0.0f, 0.0f,
        256.0f, 0.0f,   1.0f, 0.0f,
        256.0f, 256.0f, 1.0f, 1.0f,
        0.0f,   256.0f, 0.0f, 1.0f
    };

    const std::array<VertexBufferLayout::Attribute, 1> circleAttributes
    {
        VertexBufferLayout::Attribute::Float4
    };

    GlBuffer circleVbo(buffers[2], GL_ARRAY_BUFFER);
    circleVbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    auto circleVao = std::make_unique<VertexArrayObject>(vertexArrays[1]);
    circleVao->addVertexBuffer(circleVbo, circleAttributes);

//  Shaders
//  Heightmap
    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("heightmap.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("heightmap.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto heightmapProgram = std::make_unique<ShaderProgram>();
    if(!heightmapProgram->link(shaders)) return -1;

    glUseProgram(heightmapProgram->getHandle());
    glUniform1i(heightmapProgram->getUniformLocation("cracked_earth"), 0);
    glUniform1i(heightmapProgram->getUniformLocation("rock"), 1);
    glUniform1i(heightmapProgram->getUniformLocation("grass"), 2);
    glUniform1i(heightmapProgram->getUniformLocation("clover"), 3);
    glUseProgram(0);

//  Circle
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("circle.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("circle.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto circleProgram = std::make_unique<ShaderProgram>();
    if(!circleProgram->link(shaders)) return -1;

    glUseProgram(circleProgram->getHandle());
    glUniform1i(circleProgram->getUniformLocation("circleSampler"), 0);
    glUseProgram(0);

    auto getHeightInPoint = [mapWidth, mapDepth, &heightmap](float x, float z) -> float
    {
        if((x >= 0) && (x < mapWidth) && (z >= 0) && (z < mapDepth))
        {
            int32_t cX = static_cast<int32_t>(x);
            int32_t cZ = static_cast<int32_t>(z);

            const float* h = heightmap.data() + (static_cast<int32_t>(z) * mapWidth + static_cast<int32_t>(x));

            float h0 = *h;
            float h1 = *(h + 1);
            float h2 = *(h + mapWidth);
            float h3 = *(h + mapWidth + 1);

            float h5 = ((1 - (x - cX)) * h0 + (x - cX) * h1);
            float h6 = ((1 - (x - cX)) * h2 + (x - cX) * h3);

            return (1 - (z - cZ)) * h5 + (z - cZ) * h6;
        }

        return 0.f;
    };

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
                camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));
                glViewport(0, 0, width, height);
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            window.close();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            camera->processKeyboard(PerspectiveCamera::Forward, 0.1f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            camera->processKeyboard(PerspectiveCamera::Left, 0.1f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            camera->processKeyboard(PerspectiveCamera::Backward, 0.1f);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            camera->processKeyboard(PerspectiveCamera::Right, 0.1f);

//  Heightmap
		auto playerPos = camera->getPosition();
		playerPos.y = getHeightInPoint(playerPos.x, playerPos.z) + 1.7f;
		camera->setPosition(playerPos);

        const auto [xpos, ypos] = sf::Mouse::getPosition();
		auto [xt, yt] = window.getPosition();

		xt += width >> 1;
		yt += height >> 1;

		camera->processMouseMovement((xt - xpos) * 0.125f, (yt - ypos) * 0.125f);

        sf::Mouse::setPosition({xt, yt});

		uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(heightmapProgram->getHandle());

        glBindTextureUnit(0, texCrackedEarth.handle);
        glBindTextureUnit(1, texRock.handle);
        glBindTextureUnit(2, texGrass.handle);
        glBindTextureUnit(3, texClover.handle);

		const uint32_t numStrips = mapWidth - 1;
		const uint32_t numTrisPerStrip = mapWidth * 2 - 2;

		glBindVertexArray(heightmapVao->getHandle());
    
        for (uint32_t strip = 0; strip < numStrips; ++strip)
            glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT, reinterpret_cast<const void*>(sizeof(GLuint) * (numTrisPerStrip + 2) * strip));
    
        glBindTextureUnit(0, 0);
        glBindTextureUnit(1, 0);
        glBindTextureUnit(2, 0);
        glBindTextureUnit(3, 0);

        glBindVertexArray(0);
        glUseProgram(0);

//  Circles
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glUseProgram(circleProgram->getHandle());
        glBindVertexArray(circleVao->getHandle());

        int32_t halfW = imageCircleOff.width / 2;
        int32_t halfH = imageCircleOff.height / 2;
        static float rotation = 0;

        orthoCamera->setOrigin(halfW, halfH);
        orthoCamera->setPosition(halfW, height - halfH);
        orthoCamera->setRotation(-rotation);
        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(orthoCamera->getModelViewProjectionMatrix())));

        glBindTextureUnit(0, texCircleOff.handle);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindTextureUnit(0, 0);

        orthoCamera->setPosition(width - imageCircleOff.width + halfW, height - halfH);
        orthoCamera->setRotation(rotation);
        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(orthoCamera->getModelViewProjectionMatrix())));

        rotation += 0.3f;

        glBindTextureUnit(0, texCircleOn.handle);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindTextureUnit(0, 0);

        glBindVertexArray(0);
        glUseProgram(0);

        window.display();
    }

    return 0;
}