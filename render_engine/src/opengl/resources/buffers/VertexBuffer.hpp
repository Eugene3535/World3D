#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <span>
#include <vector>

#include "opengl/resources/buffers/GlBuffer.hpp"

// A vertex attribute is an input variable to a shader that is supplied with per - vertex data.
// In OpenGL core profile, they are specified as in variables in a vertex shader and are backed by a GL_ARRAY_BUFFER.
// These variable can contain, for example, positions, normals or texture coordinates.
// A vertex is the combination of all the vertex attributes that belong together.


class RE_API AttributeInfo
{
public:
    enum Type
    {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
    };

    AttributeInfo(Type type) noexcept;

    Type type;
    uint32_t componentType;
    size_t componentsCount;
    size_t sizeInBytes;
    size_t offset;
};


class RE_API VertexBufferLayout
{
public:
    VertexBufferLayout(std::span<AttributeInfo> attributes) noexcept;

    const std::vector<AttributeInfo>& getAttributes() const noexcept;
    size_t getStride() const noexcept;

private:
    std::vector<AttributeInfo> m_attributes;
    size_t m_stride;
};


class RE_API VertexBuffer:
    public GlBuffer
{
public:
    VertexBuffer(uint32_t handle, const VertexBufferLayout& layout) noexcept;
    ~VertexBuffer() noexcept;

    const VertexBufferLayout& getLayout() const noexcept;

private:
    VertexBufferLayout m_layout;
};

#endif // !VERTEX_BUFFER_HPP