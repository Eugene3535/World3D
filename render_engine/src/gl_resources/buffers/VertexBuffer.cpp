#include <glad/glad.h>

#include "VertexBuffer.hpp"


static constexpr uint32_t shaderAttributeTypeToComponentCount(const AttributeInfo::Type type) noexcept
{
    switch (type)
    {
        case AttributeInfo::Type::Float:
        case AttributeInfo::Type::Int:
            return 1;

        case AttributeInfo::Type::Float2:
        case AttributeInfo::Type::Int2:
            return 2;

        case AttributeInfo::Type::Float3:
        case AttributeInfo::Type::Int3:
            return 3;

        case AttributeInfo::Type::Float4:
        case AttributeInfo::Type::Int4:
            return 4;
    }

    return 0;
}


static constexpr size_t shaderAttributeTypeSizeOf(const AttributeInfo::Type type) noexcept
{
    switch (type)
    {
        case AttributeInfo::Type::Float:
        case AttributeInfo::Type::Float2:
        case AttributeInfo::Type::Float3:
        case AttributeInfo::Type::Float4:
            return sizeof(GLfloat) * shaderAttributeTypeToComponentCount(type);

        case AttributeInfo::Type::Int:
        case AttributeInfo::Type::Int2:
        case AttributeInfo::Type::Int3:
        case AttributeInfo::Type::Int4:
            return sizeof(GLint) * shaderAttributeTypeToComponentCount(type);
    }

    return 0;
}


static constexpr unsigned int shaderAttributeTypeToComponentType(const AttributeInfo::Type type) noexcept
{
    switch (type)
    {
        case AttributeInfo::Type::Float:
        case AttributeInfo::Type::Float2:
        case AttributeInfo::Type::Float3:
        case AttributeInfo::Type::Float4:
            return GL_FLOAT;

        case AttributeInfo::Type::Int:
        case AttributeInfo::Type::Int2:
        case AttributeInfo::Type::Int3:
        case AttributeInfo::Type::Int4:
            return GL_INT;
    }

    return GL_FLOAT;
}


static constexpr GLenum usageToGLenum(const VertexBuffer::Usage usage) noexcept
{
    switch (usage)
    {
        case VertexBuffer::Usage::Static:  return GL_STATIC_DRAW;
        case VertexBuffer::Usage::Dynamic: return GL_DYNAMIC_DRAW;
        case VertexBuffer::Usage::Stream:  return GL_STREAM_DRAW;
    }

    return GL_STREAM_DRAW;
}


AttributeInfo::AttributeInfo(const AttributeInfo::Type attrType) noexcept:
    type(attrType),
    componentType(shaderAttributeTypeToComponentType(attrType)),
    componentsCount(shaderAttributeTypeToComponentCount(attrType)),
    sizeInBytes(shaderAttributeTypeSizeOf(attrType)),
    offset(0)
{

}


VertexBufferLayout::VertexBufferLayout(std::initializer_list<AttributeInfo> attributes) noexcept :
    m_attributes(attributes),
    m_stride(0)
{
    size_t offset = 0;

    for (auto& attribute : m_attributes)
    {
        attribute.offset = offset;
        offset += attribute.sizeInBytes;
        m_stride += attribute.sizeInBytes;
    }
}


const std::vector<AttributeInfo>& VertexBufferLayout::getAttributes() const noexcept
{
    return m_attributes;
}


size_t VertexBufferLayout::getStride() const noexcept
{
    return m_stride;
}


VertexBuffer::VertexBuffer(const void* data, size_t size, const VertexBufferLayout& layout, Usage usage) noexcept:
    m_layout(layout)
{
    glGenBuffers(1, &m_handle);
    glBindBuffer(GL_ARRAY_BUFFER, m_handle);
    glBufferData(GL_ARRAY_BUFFER, size, data, usageToGLenum(usage));
}


VertexBuffer::VertexBuffer(VertexBuffer&& vertex_buffer) noexcept :
    m_handle(vertex_buffer.m_handle),
    m_layout(vertex_buffer.m_layout)
{
    vertex_buffer.m_handle = 0;
}


VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vertex_buffer) noexcept
{
    m_handle = vertex_buffer.m_handle;
    vertex_buffer.m_handle = 0;

    return *this;
}


VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_handle);
}


uint32_t VertexBuffer::getHandle() const noexcept
{
    return m_handle;
}


const VertexBufferLayout& VertexBuffer::getLayout() const noexcept
{
    return m_layout;
}