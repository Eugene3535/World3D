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
#include "scenes/paths/PathDemo.hpp"


PathDemo::PathDemo(sf::Window& window) noexcept:
    DemoScene(window)
{

}


PathDemo::~PathDemo()
{

}


bool PathDemo::init(GlResourceHolder& holder) noexcept
{
    StbImage imgSnow;     imgSnow.loadFromFile(FileProvider::findPathToFile("snow.png"));
    StbImage imgPavement; imgPavement.loadFromFile(FileProvider::findPathToFile("pavement.jpg"));
    StbImage imgPath;     imgPath.loadFromFile(FileProvider::findPathToFile("test.png"));

    auto textures = holder.create<Texture, 3>();

    m_textureSnow     = std::make_unique<Texture>();
    m_texturePavement = std::make_unique<Texture>();
    m_texturePath     = std::make_unique<Texture>();

    m_textureSnow->handle = textures[0];
    m_texturePavement->handle = textures[1];
    m_texturePath->handle = textures[2];

    m_textureSnow->loadFromImage(imgSnow, true, true);
    m_texturePavement->loadFromImage(imgPavement, true, true);
    m_texturePath->loadFromImage(imgPath, true, true);

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

    std::array<uint32_t, 2> buffers = holder.create<GlBuffer, 2>();
    std::array<uint32_t, 1> vertexArrays = holder.create<VertexArrayObject, 1>();

    GlBuffer vbo(buffers[0], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    m_vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    m_vao->addVertexBuffer(vbo, attributes);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile(FileProvider::findPathToFile("ground.vert"), GL_VERTEX_SHADER))   return false;
    if(!shaders[1].loadFromFile(FileProvider::findPathToFile("ground.frag"), GL_FRAGMENT_SHADER)) return false;

    m_program = std::make_unique<ShaderProgram>();
    if(!m_program->link(shaders)) return false;

    glUseProgram(m_program->getHandle());

    if(GLint uniformLoc = m_program->getUniformLocation("snowSampler"); uniformLoc != -1)
        glUniform1i(uniformLoc, 0);

    if(GLint uniformLoc = m_program->getUniformLocation("pavementSampler"); uniformLoc != -1)
        glUniform1i(uniformLoc, 1);

    if(GLint uniformLoc = m_program->getUniformLocation("pathSampler"); uniformLoc != -1)
        glUniform1i(uniformLoc, 2);

    glUseProgram(0);

    m_uniformBuffer = std::make_unique<GlBuffer>(buffers[1], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(glm::mat4));

    auto [width, height] = m_window.getSize();
    m_camera = std::make_unique<PerspectiveCamera>();
    m_camera->setDrawDistance(500);
    m_camera->updateProjectionMatrix(static_cast<float>(width) / static_cast<float>(height));
    m_camera->setPosition(3, 3, 3);

    return true;
}


void PathDemo::update(const sf::Time& dt) noexcept
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        m_camera->processKeyboard(PerspectiveCamera::Forward, 1);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        m_camera->processKeyboard(PerspectiveCamera::Left, 1);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        m_camera->processKeyboard(PerspectiveCamera::Backward, 1);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        m_camera->processKeyboard(PerspectiveCamera::Right, 1);

    auto playerPos = m_camera->getPosition();
    playerPos.y = 30;
    m_camera->setPosition(playerPos);

    const auto [xpos, ypos] = sf::Mouse::getPosition();
    const auto [width, height] = m_window.getSize();
    auto [xt, yt] = m_window.getPosition();

    xt += width >> 1;
    yt += height >> 1;

    m_camera->processMouseMovement((xt - xpos) * 0.125f, (yt - ypos) * 0.125f);

    sf::Mouse::setPosition({xt, yt});

    m_uniformBuffer->update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(m_camera->getModelViewProjectionMatrix())));
}


void PathDemo::draw() noexcept
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program->getHandle());

    glBindTextureUnit(0, m_textureSnow->handle);
    glBindTextureUnit(1, m_texturePavement->handle);
    glBindTextureUnit(2, m_texturePath->handle);

    glBindVertexArray(m_vao->getHandle());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glBindTextureUnit(0, 0);
    glBindTextureUnit(1, 0);
    glBindTextureUnit(2, 0);

    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);
}