#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <memory>

#include "Image.hpp"
#include "Texture2D.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "VertexArray.hpp"
#include "Scene.hpp"

class Heightmap:
    public Scene
{
public:
    Heightmap(void* handle) noexcept;
    ~Heightmap() noexcept;

    void draw() noexcept override;

private:
    Image m_imageMap;
    Image m_imgCrackedEarth;
    Image m_imgRock;
    Image m_imgGrass;
    Image m_imgClover;

    std::unique_ptr<Texture2D> m_texCrackedEarth;
    std::unique_ptr<Texture2D> m_texRock;
    std::unique_ptr<Texture2D> m_texGrass;
    std::unique_ptr<Texture2D> m_texClover;

    std::unique_ptr<ShaderProgram> m_program;
    Camera m_camera;

    std::vector<float> m_heightmap;
    uint32_t m_mapDepth;
    uint32_t m_mapWidth;

    std::unique_ptr<VertexBuffer> m_vbo;
    std::unique_ptr<IndexBuffer> m_ebo;
    std::unique_ptr<VertexArray> m_vao;
};

#endif // !HEIGHTMAP_HPP