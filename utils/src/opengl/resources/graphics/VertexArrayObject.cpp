#include "opengl/resources/graphics/VertexArrayObject.hpp"

#include <glad/glad.h>


static constexpr GLenum primitiveTypeToGlEnum(PrimitiveType primitive)
{
	switch (primitive)
	{
		case PrimitiveType::Points:                 return GL_POINTS; 
		case PrimitiveType::LineStrip:              return GL_LINE_STRIP;
		case PrimitiveType::LineLoop:               return GL_LINE_LOOP; 
		case PrimitiveType::Lines:                  return GL_LINES; 
		case PrimitiveType::LineStripAdjacency:     return GL_LINE_STRIP_ADJACENCY; 
		case PrimitiveType::LinesAdjacency:         return GL_LINES_ADJACENCY; 
		case PrimitiveType::TriangleStrip:          return GL_TRIANGLE_STRIP; 
		case PrimitiveType::TriangleFan:            return GL_TRIANGLE_FAN; 
		case PrimitiveType::Triangles:              return GL_TRIANGLES; 
		case PrimitiveType::TriangleStripAdjacency: return GL_TRIANGLE_STRIP_ADJACENCY; 
		case PrimitiveType::TrianglesAdjacency:     return GL_TRIANGLES_ADJACENCY; 
		case PrimitiveType::Patches:                return GL_PATCHES;		

		default: return GL_POINTS;
	}	
}


VertexArrayObject::VertexArrayObject(uint32_t handle) noexcept:
	GlResource(handle),
	m_attributeCount(0),
	m_indexCount(0)
{
	
}


VertexArrayObject::~VertexArrayObject() noexcept = default;


void VertexArrayObject::bind(VertexArrayObject* vao) noexcept
{
	glBindVertexArray(vao ? vao->m_handle : 0);
}


void VertexArrayObject::addVertexBuffer(const VertexBuffer& vertexBuffer) noexcept
{
	bind(this);

	for (const auto& attribute : vertexBuffer.getLayout().getAttributes())
	{
		glEnableVertexAttribArray(m_attributeCount);
		glBindVertexBuffer(m_attributeCount, vertexBuffer.getHandle(), attribute.offset, static_cast<int32_t>(vertexBuffer.getLayout().getStride()));
		glVertexAttribFormat(m_attributeCount, static_cast<int32_t>(attribute.componentsCount), attribute.componentType, GL_FALSE, 0);
		glVertexAttribBinding(m_attributeCount, m_attributeCount);

		++m_attributeCount;
	}

	bind(nullptr);
}


void VertexArrayObject::setIndexBuffer(IndexBuffer& indexBuffer) noexcept
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


uint32_t VertexArrayObject::getIndexCount() const noexcept
{
	return m_indexCount;
}


void VertexArrayObject::drawArrays(PrimitiveType mode, int32_t first, int32_t count) noexcept
{
	glDrawArrays(primitiveTypeToGlEnum(mode), first, count);
}


void VertexArrayObject::drawElements(PrimitiveType mode, int32_t count, const void* indices) noexcept
{
	if(m_indexCount)
		glDrawElements(primitiveTypeToGlEnum(mode), count, GL_UNSIGNED_INT, indices);
}