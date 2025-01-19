#include "opengl/resources/shared/VertexArray.hpp"

#include <glad/glad.h>

VertexArray::VertexArray(uint32_t handle) noexcept:
	GlResource(),
	m_attributeCount(0),
	m_indexCount(0)
{
	m_handle = handle;
}


VertexArray::~VertexArray() noexcept = default;


void VertexArray::bind(VertexArray* vao) noexcept
{
	glBindVertexArray(vao ? vao->m_handle : 0);
}


void VertexArray::addVertexBuffer(const VertexBuffer& vertexBuffer) noexcept
{
	bind(this);

	for (const auto& attribute : vertexBuffer.getLayout().getAttributes())
	{
		glEnableVertexAttribArray(m_attributeCount);

		glBindVertexBuffer(m_attributeCount,
			vertexBuffer.getHandle(),
			attribute.offset,
			static_cast<int32_t>(vertexBuffer.getLayout().getStride()));

		glVertexAttribFormat(m_attributeCount,
			static_cast<int32_t>(attribute.componentsCount),
			attribute.componentType,
			GL_FALSE,
			0);

		glVertexAttribBinding(m_attributeCount, m_attributeCount);

		++m_attributeCount;
	}

	bind(nullptr);
}


void VertexArray::setIndexBuffer(IndexBuffer& indexBuffer) noexcept
{
	if(indexBuffer.getCount() > 0)
	{
		bind(this);
		indexBuffer.bind(true);
		m_indexCount = indexBuffer.getCount();
		bind(nullptr);
		indexBuffer.bind(false);
	}
}


uint32_t VertexArray::getIndexCount() const noexcept
{
	return m_indexCount;
}