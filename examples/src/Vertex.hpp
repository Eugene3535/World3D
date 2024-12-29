#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

struct Vertex
{
	Vertex() noexcept;
	Vertex(const glm::vec3& vert, const glm::vec2& tex) noexcept;

	glm::vec3 position;
	glm::vec2 uv;
	// glm::vec3 normal;
};

#endif // !VERTEX_HPP