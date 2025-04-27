#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "camera/perspective/PerspectiveCamera.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
#include "scenes/heightmap/HeightmapDemo.hpp"


HeightmapDemo::HeightmapDemo(sf::Window& window) noexcept:
    DemoScene(window)
{

}


HeightmapDemo::~HeightmapDemo()
{

}


bool HeightmapDemo::init(GlResourceHolder& holder) noexcept
{
    auto [width, height] = m_window.getSize();

    std::array<uint32_t, 1> buffer = holder.create<GlBuffer, 1>();

    m_uniformBuffer = std::make_unique<GlBuffer>(buffer[0], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(glm::mat4));

    m_orthoCamera = std::make_unique<OrthogonalCamera>();
    m_orthoCamera->setupProjectionMatrix(width, height);

    m_perspectiveCamera = std::make_unique<PerspectiveCamera>();
    m_perspectiveCamera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));
    m_perspectiveCamera->setDrawDistance(100);
    m_perspectiveCamera->setPosition(30, 3, 30);

    m_heightmapImage = std::make_unique<StbImage>();
    if(!m_heightmapImage->loadFromFile(FileProvider::findPathToFile("heightmap.png"))) return false;

    StbImage imgCrackedEarth; if(!imgCrackedEarth.loadFromFile(FileProvider::findPathToFile("cracked_earth.jpg"))) return false;
    StbImage imgRock;         if(!imgRock.loadFromFile(FileProvider::findPathToFile("rock.jpg")))                  return false;
    StbImage imgGrass;        if(!imgGrass.loadFromFile(FileProvider::findPathToFile("grass.jpg")) )               return false;
    StbImage imgClover;       if(!imgClover.loadFromFile(FileProvider::findPathToFile("clover.png")))              return false;
    StbImage imageCircleOff;  if(!imageCircleOff.loadFromFile(FileProvider::findPathToFile("circle_off.png")))     return false;
    StbImage imageCircleOn;   if(!imageCircleOn.loadFromFile(FileProvider::findPathToFile("circle_on.png")))       return false;

    const auto textures = holder.create<Texture, 6>();

    m_texCrackedEarth = std::make_unique<Texture>(textures[0], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_texRock         = std::make_unique<Texture>(textures[1], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_texGrass        = std::make_unique<Texture>(textures[2], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_texClover       = std::make_unique<Texture>(textures[3], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_texCircleOff    = std::make_unique<Texture>(textures[4], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_texCircleOn     = std::make_unique<Texture>(textures[5], GL_TEXTURE_2D, 0, 0, 0, false, false);

    if(!m_texCrackedEarth->loadFromImage(imgCrackedEarth, true, true)) return false;
    if(!m_texRock->loadFromImage(imgRock, true, true))                 return false;
    if(!m_texGrass->loadFromImage(imgGrass, true, true))               return false;
    if(!m_texClover->loadFromImage(imgClover, true, true))             return false;
    if(!m_texCircleOff->loadFromImage(imageCircleOff, false, false))   return false;
    if(!m_texCircleOn->loadFromImage(imageCircleOn, false, false))     return false;

    const uint8_t* pixels   = m_heightmapImage->pixels.get();
    const uint32_t mapDepth = m_heightmapImage->width;
    const uint32_t mapWidth = m_heightmapImage->height;

    m_heightmap.resize(mapDepth * mapWidth);
    std::vector<float> vertices(mapDepth * mapWidth * 5);
    std::vector<uint32_t> indices;
    indices.reserve(static_cast<size_t>(mapDepth * mapWidth * 2));

    size_t index = 0;
    size_t stride = 0;

    for (size_t z = 0; z < mapDepth; ++z)
    {
        for (size_t x = 0; x < mapWidth; ++x)
        {
            const uint8_t* pixel = pixels + ((z * mapWidth + x) * m_heightmapImage->bytePerPixel);
            int32_t y = static_cast<int32_t>(pixel[0]);
            float Y = y * 0.03f;

            float* vertex = vertices.data() + stride;

            vertex[0] = x;
            vertex[1] = Y;
            vertex[2] = z;
            vertex[3] = x * 0.3f;
            vertex[4] = z * 0.3f;

            m_heightmap[index] = Y;

            ++index;
            stride += 5;
        }
    }

    for(uint32_t i = 0; i < mapDepth - 1; ++i)
    {
        for(uint32_t j = 0; j < mapWidth; ++j)
        {
            indices.push_back(mapWidth * i + j);
            indices.push_back(mapWidth * (i + 1) + j);       
        }
    }

//  Allocate buffers
    const auto buffers = holder.create<GlBuffer, 3>();
    const auto vertexArrays = holder.create<VertexArrayObject, 2>();

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

    m_heightmapVao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    m_heightmapVao->addVertexBuffer(heightmapVbo, heightmapAttributes);
    m_heightmapVao->setElementBuffer(heightmapEbo);

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

    m_circleVao = std::make_unique<VertexArrayObject>(vertexArrays[1]);
    m_circleVao->addVertexBuffer(circleVbo, circleAttributes);

//  Shaders
//  Heightmap
    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("heightmap.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("heightmap.frag"), GL_FRAGMENT_SHADER)) return -1;

    m_heightmapProgram = std::make_unique<ShaderProgram>();

    if(!m_heightmapProgram->link(shaders)) 
        return false;

    glUseProgram(m_heightmapProgram->getHandle());
    glUniform1i(m_heightmapProgram->getUniformLocation("cracked_earth"), 0);
    glUniform1i(m_heightmapProgram->getUniformLocation("rock"), 1);
    glUniform1i(m_heightmapProgram->getUniformLocation("grass"), 2);
    glUniform1i(m_heightmapProgram->getUniformLocation("clover"), 3);
    glUseProgram(0);

//  Circle
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("circle.vert"), GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("circle.frag"), GL_FRAGMENT_SHADER)) return -1;

    m_circleProgram = std::make_unique<ShaderProgram>();
    if(!m_circleProgram->link(shaders)) return -1;

    glUseProgram(m_circleProgram->getHandle());
    glUniform1i(m_circleProgram->getUniformLocation("circleSampler"), 0);
    glUseProgram(0);

    return true;
}


void HeightmapDemo::update(const sf::Time& dt) noexcept
{
    auto getHeightInPoint = [this](float x, float z) -> float
    {
        const uint32_t mapDepth = m_heightmapImage->width;
        const uint32_t mapWidth = m_heightmapImage->height;

        if((x >= 0) && (x < mapWidth) && (z >= 0) && (z < mapDepth))
        {
            int32_t cX = static_cast<int32_t>(x);
            int32_t cZ = static_cast<int32_t>(z);

            const float* h = m_heightmap.data() + (static_cast<int32_t>(z) * mapWidth + static_cast<int32_t>(x));

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

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        m_perspectiveCamera->processKeyboard(PerspectiveCamera::Forward, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        m_perspectiveCamera->processKeyboard(PerspectiveCamera::Left, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        m_perspectiveCamera->processKeyboard(PerspectiveCamera::Backward, 0.1f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        m_perspectiveCamera->processKeyboard(PerspectiveCamera::Right, 0.1f);

    //  Heightmap
    auto playerPos = m_perspectiveCamera->getPosition();
    playerPos.y = getHeightInPoint(playerPos.x, playerPos.z) + 1.7f;
    m_perspectiveCamera->setPosition(playerPos);

    const auto [xpos, ypos] = sf::Mouse::getPosition();
    const auto [width, height] = m_window.getSize();
    auto [xt, yt] = m_window.getPosition();

    xt += width >> 1;
    yt += height >> 1;

    m_perspectiveCamera->processMouseMovement((xt - xpos) * 0.125f, (yt - ypos) * 0.125f);

    sf::Mouse::setPosition({xt, yt});

    m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(m_perspectiveCamera->getModelViewProjectionMatrix())));
}


void HeightmapDemo::draw() noexcept
{
    m_window.setMouseCursorVisible(false);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_heightmapProgram->getHandle());

    glBindTextureUnit(0, m_texCrackedEarth->handle);
    glBindTextureUnit(1, m_texRock->handle);
    glBindTextureUnit(2, m_texGrass->handle);
    glBindTextureUnit(3, m_texClover->handle);

    const uint32_t mapDepth = m_heightmapImage->width;
    const uint32_t mapWidth = m_heightmapImage->height;
    const auto [width, height] = m_window.getSize();

    const uint32_t numStrips = mapWidth - 1;
    const uint32_t numTrisPerStrip = mapWidth * 2 - 2;

    glBindVertexArray(m_heightmapVao->getHandle());

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

    glUseProgram(m_circleProgram->getHandle());
    glBindVertexArray(m_circleVao->getHandle());

    int32_t halfW = m_texCircleOff->width / 2;
    int32_t halfH = m_texCircleOff->height / 2;
    static float rotation = 0;

    m_orthoCamera->setOrigin(halfW, halfH);
    m_orthoCamera->setPosition(halfW, height - halfH);
    m_orthoCamera->setRotation(-rotation);
    m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(m_orthoCamera->getModelViewProjectionMatrix())));

    glBindTextureUnit(0, m_texCircleOff->handle);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTextureUnit(0, 0);

    m_orthoCamera->setPosition(width - m_texCircleOff->width + halfW, height - halfH);
    m_orthoCamera->setRotation(rotation);
    m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(m_orthoCamera->getModelViewProjectionMatrix())));

    rotation += 0.3f;

    glBindTextureUnit(0, m_texCircleOn->handle);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTextureUnit(0, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    m_window.setMouseCursorVisible(true);
}