#include "Vertex.hpp"

Vertex::Vertex() noexcept:
    position(),
    uv()
{

}

Vertex::Vertex(const glm::vec3& vert, const glm::vec2& tex) noexcept:
    position(vert),
    uv(tex)
{

}