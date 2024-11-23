#include <cstring>

#include "graphics/VertexBuffer.hpp"

VertexBuffer::VertexBuffer() noexcept:
    m_vao(0),
    m_vbo(0),
    m_ibo(0),
	m_vertex_count(0),
    m_index_count(0)
{
}

void VertexBuffer::create(const std::vector<Vertex>& vertices, const std::vector<glm::u32>& indices) noexcept
{
	if(m_vao) return;

	m_vertex_count = static_cast<glm::u32>(vertices.size());
	m_index_count  = static_cast<glm::u32>(indices.size());

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	if(!indices.empty())
		glGenBuffers(1, &m_ibo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size()) * sizeof(Vertex), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);  

	if(!indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size()) * sizeof(glm::u32), static_cast<const void*>(indices.data()), GL_STATIC_DRAW);
	}

	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, uv)));

	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));
}

void VertexBuffer::draw(glm::u32 primitive_type) noexcept
{
	if(m_vao)
	{
		glBindVertexArray(m_vao);

		if (m_ibo) 
		{
			glDrawElements(primitive_type, m_index_count, GL_UNSIGNED_INT, nullptr);
		}
		else 
		{
			glDrawArrays(primitive_type, 0, m_vertex_count);
		}

		glBindVertexArray(0);
	}
}

void VertexBuffer::reset() noexcept
{
	if(m_vao) glDeleteVertexArrays(1, &m_vao);
	if(m_vbo) glDeleteBuffers(1, &m_vbo);
    if(m_ibo) glDeleteBuffers(1, &m_ibo);

	std::memset(this, 0, sizeof(VertexBuffer));
}

bool VertexBuffer::is_loaded() const noexcept
{
	return (m_vao != 0);
}