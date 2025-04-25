#ifndef DUNE_DEMO_HPP
#define DUNE_DEMO_HPP

#include <memory>

#include "files/StbImage.hpp"
#include "scenes/DemoScene.hpp"


class DuneDemo final:
    public DemoScene
{
public:
    DuneDemo(class sf::Window& window) noexcept;
    ~DuneDemo();

    bool init(class GlResourceHolder& holder) noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

private:
    StbImage m_mapMask;

    std::unique_ptr<class Texture> m_texture0;
    std::unique_ptr<class Texture> m_texture1;
    std::unique_ptr<class Texture> m_texture2;
    std::unique_ptr<class Texture> m_texture3;
    std::unique_ptr<class Texture> m_texture4;

    std::unique_ptr<class ShaderProgram>     m_program;
    std::unique_ptr<class VertexArrayObject> m_vao;
    std::unique_ptr<class OrthogonalCamera>  m_camera;
    std::unique_ptr<class GlBuffer>          m_uniformBuffer;
};


#endif // !DUNE_DEMO_HPP