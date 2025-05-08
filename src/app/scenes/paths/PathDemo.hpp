#ifndef PATH_DEMO_HPP
#define PATH_DEMO_HPP

#include <memory>

#include "scenes/DemoScene.hpp"


class PathDemo final:
    public DemoScene
{
public:
    PathDemo(class sf::Window& window) noexcept;
    ~PathDemo();

    bool init() noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

private:
    std::unique_ptr<class ShaderProgram>     m_program;
    std::unique_ptr<class VertexArrayObject> m_vao;
    std::unique_ptr<class Texture>           m_textureSnow;
    std::unique_ptr<class Texture>           m_texturePavement;
    std::unique_ptr<class Texture>           m_texturePath;
    std::unique_ptr<class PerspectiveCamera> m_camera;
    std::unique_ptr<class GlBuffer>          m_uniformBuffer;
};


#endif // !PATH_DEMO_HPP