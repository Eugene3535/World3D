#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <memory>

#include "files/Image.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "opengl/resources/graphics/Texture2D.hpp"
#include "opengl/resources/graphics/VertexArrayObject.hpp"
#include "Scene.hpp"

class Heightmap:
    public Scene
{
public:
    Heightmap(class GlResourceHolder* resourceHolder) noexcept;
    ~Heightmap() noexcept;

    void draw() noexcept override;

    float getHeightInPoint(float x, float z) const noexcept;

private:
    std::unique_ptr<Texture2D> m_texCrackedEarth;
    std::unique_ptr<Texture2D> m_texRock;
    std::unique_ptr<Texture2D> m_texGrass;
    std::unique_ptr<Texture2D> m_texClover;

    std::unique_ptr<ShaderProgram> m_program;

    std::vector<float> m_heightmap;
    uint32_t m_mapDepth;
    uint32_t m_mapWidth;

    std::unique_ptr<VertexBuffer> m_vbo;
    std::unique_ptr<IndexBuffer> m_ebo;
    std::unique_ptr<VertexArrayObject> m_vao;
};

#endif // !HEIGHTMAP_HPP