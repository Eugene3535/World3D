#ifndef LIGHT_DEMO_HPP
#define LIGHT_DEMO_HPP

#include <memory>
#include <vector>

#include "graphics/meshes/Mesh.hpp"
#include "scenes/DemoScene.hpp"


#include <memory>

#include <SFML/System/Vector2.hpp>

#include "scenes/DemoScene.hpp"


class LightDemo final:
    public DemoScene
{
public:
    LightDemo(class sf::Window& window) noexcept;
    ~LightDemo();

    bool init() noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

    void processMouseScroll(float delta) noexcept;
    void processMouseMovement(float dx, float dy) noexcept;

public:
    std::unique_ptr<class ShaderProgram>     m_program;
    std::unique_ptr<class VertexArrayObject> m_vao;
    std::unique_ptr<class Texture>           m_texture;
    std::unique_ptr<class Camera3D>          m_camera;
    std::unique_ptr<class GlBuffer>          m_uniformBuffer;

    float m_mouseScrollDelta;
    sf::Vector2f m_mouseMovementDelta;
};

#endif // !LIGHT_DEMO_HPP