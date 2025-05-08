#ifndef ORBIT_DEMO_HPP
#define ORBIT_DEMO_HPP

#include <memory>

#include <SFML/System/Vector2.hpp>

#include "scenes/DemoScene.hpp"


class OrbitDemo final:
    public DemoScene
{
public:
    OrbitDemo(class sf::Window& window) noexcept;
    ~OrbitDemo();

    bool init() noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

    void processMouseScroll(float delta) noexcept;

private:
    std::unique_ptr<class ShaderProgram>     m_program;
    std::unique_ptr<class VertexArrayObject> m_vao;
    std::unique_ptr<class Texture>           m_texture;
    std::unique_ptr<class OrbitCamera>       m_camera;
    std::unique_ptr<class GlBuffer>          m_uniformBuffer;

    sf::Vector2f m_previousMouse;
    sf::Vector2f m_currentMouse;

    float m_mouseScrollDelta;

    bool m_isRotationMode;
    bool m_isMovementMode;
};


#endif // !ORBIT_DEMO_HPP