#include <glad/glad.h>

#include "IndexBuffer.hpp"


IndexBuffer::IndexBuffer(const void* data, size_t count, VertexBuffer::Usage usage):
    m_count(count)
{
    auto usageToGLenum = [](const VertexBuffer::Usage usage) noexcept -> GLenum
    {
        switch (usage)
        {
            case VertexBuffer::Usage::Static:  return GL_STATIC_DRAW;
            case VertexBuffer::Usage::Dynamic: return GL_DYNAMIC_DRAW;
            case VertexBuffer::Usage::Stream:  return GL_STREAM_DRAW;

            default: return GL_STREAM_DRAW;
        }
    };

    glGenBuffers(1, &m_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, usageToGLenum(usage));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


IndexBuffer::IndexBuffer(IndexBuffer&& indexBuffer) noexcept: 
    m_handle(indexBuffer.m_handle),
    m_count(indexBuffer.m_count)
{
    indexBuffer.m_handle = 0;
    indexBuffer.m_count = 0;
}


IndexBuffer& IndexBuffer::operator = (IndexBuffer&& indexBuffer) noexcept
{
    m_handle = indexBuffer.m_handle;
    m_count = indexBuffer.m_count;
    indexBuffer.m_handle = 0;
    indexBuffer.m_count = 0;

    return *this;
}


IndexBuffer::~IndexBuffer() noexcept
{
    glDeleteBuffers(1, &m_handle);
}


void IndexBuffer::bind(IndexBuffer* buffer) noexcept
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer ? buffer->m_handle : 0);
}


uint32_t IndexBuffer::getCount() const noexcept
{
    return m_count;
}