#ifndef HEIGHTMAP_DEMO_HPP
#define HEIGHTMAP_DEMO_HPP

#include <memory>
#include <vector>

#include "scenes/DemoScene.hpp"


class HeightmapDemo final:
    public DemoScene
{
public:
    HeightmapDemo(class sf::Window& window) noexcept;
    ~HeightmapDemo();

    bool init() noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

private:
    std::unique_ptr<class StbImage> m_heightmapImage;
    std::unique_ptr<class Texture>  m_texCrackedEarth;
    std::unique_ptr<class Texture>  m_texRock;
    std::unique_ptr<class Texture>  m_texGrass;
    std::unique_ptr<class Texture>  m_texClover;
    std::unique_ptr<class Texture>  m_texCircleOff;
    std::unique_ptr<class Texture>  m_texCircleOn;

    std::vector<float> m_heightmap;

    std::unique_ptr<class ShaderProgram> m_heightmapProgram;
    std::unique_ptr<class ShaderProgram> m_circleProgram;

    std::unique_ptr<class VertexArrayObject> m_heightmapVao;
    std::unique_ptr<class VertexArrayObject> m_circleVao;

    std::unique_ptr<class OrthogonalCamera>  m_orthoCamera;
    std::unique_ptr<class PerspectiveCamera> m_perspectiveCamera;

    std::unique_ptr<class GlBuffer> m_uniformBuffer;
};


#endif // !HEIGHTMAP_DEMO_HPP