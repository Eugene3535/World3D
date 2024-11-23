#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <vector>

#include <glad/glad.h>

#include "graphics/Vertex.hpp"

class VertexBuffer
{
public:
    VertexBuffer() noexcept;

    void create(const std::vector<Vertex>& vertices, const std::vector<glm::u32>& indices) noexcept;
    void draw(glm::u32 primitive_type = GL_TRIANGLES) noexcept;
    void reset() noexcept;

    bool is_loaded() const noexcept;

private:   
    glm::u32 m_vao;
    glm::u32 m_vbo;
    glm::u32 m_ibo;

private:
    glm::u32 m_vertex_count;
    glm::u32 m_index_count;
};

#endif // !VERTEX_BUFFER_HPP
