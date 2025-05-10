#ifndef DUNE_DEMO_HPP
#define DUNE_DEMO_HPP

#include <memory>
#include <vector>

#include "graphics/meshes/Mesh.hpp"
#include "scenes/DemoScene.hpp"


class DuneDemo final:
    public DemoScene
{
public:
    DuneDemo(class sf::Window& window) noexcept;
    ~DuneDemo();

    bool init() noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

private:
    Mesh m_mesh;
    std::vector<class Texture> m_textures;
    std::unique_ptr<class StbImage> m_mapMask;

    std::unique_ptr<class ShaderProgram>    m_program;
    std::unique_ptr<class OrthogonalCamera> m_camera;
    std::unique_ptr<class GlBuffer>         m_uniformBuffer;
};


#endif // !DUNE_DEMO_HPP