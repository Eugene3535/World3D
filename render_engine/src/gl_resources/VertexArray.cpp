#include "VertexArray.hpp"

#include <glad/glad.h>

VertexArray::VertexArray() noexcept:
	GlResource(),
	m_attributeCount(0),
	m_indexCount(0)
{
	glGenVertexArrays(1, &m_handle);
}


VertexArray::VertexArray(VertexArray&& vertexArray) noexcept :
	GlResource(std::move(vertexArray)),
	m_attributeCount(vertexArray.m_attributeCount),
	m_indexCount(0)
{
	vertexArray.m_attributeCount = 0;
}


VertexArray& VertexArray::operator=(VertexArray&& vertexArray) noexcept
{
	GlResource::operator = (std::move(vertexArray));
	m_attributeCount = vertexArray.m_handle;
	vertexArray.m_attributeCount = 0;

	return *this;
}


VertexArray::~VertexArray() noexcept
{
	glDeleteVertexArrays(1, &m_handle);
}


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
	bind(this);
	IndexBuffer::bind(&indexBuffer);
	m_indexCount = indexBuffer.getCount();
	bind(nullptr);
}


uint32_t VertexArray::getIndexCount() const noexcept
{
	return m_indexCount;
}