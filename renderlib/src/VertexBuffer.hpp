#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <vector>
#include <cstdint>


// A vertex attribute is an input variable to a shader that is supplied with per - vertex data.
// In OpenGL core profile, they are specified as in variables in a vertex shader and are backed by a GL_ARRAY_BUFFER.
// These variable can contain, for example, positions, normals or texture coordinates.
// A vertex is the combination of all the vertex attributes that belong together.


class AttributeInfo
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


class BufferLayout
{
public:
    BufferLayout(std::initializer_list<AttributeInfo> attributes) noexcept;

    const std::vector<AttributeInfo>& getAttributes() const noexcept;
    size_t getStride() const noexcept;

private:
    std::vector<AttributeInfo> m_attributes;
    size_t m_stride;
};


class VertexBuffer 
{
public:
    enum Usage
    {
        Static,
        Dynamic,
        Stream
    };

    VertexBuffer(const void* data, size_t size, const BufferLayout& layout, Usage usage = Static) noexcept;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&& vertex_buffer) noexcept;
    VertexBuffer(VertexBuffer&& vertex_buffer) noexcept;
    ~VertexBuffer() noexcept;

    uint32_t getHandle() const noexcept;
    const BufferLayout& getLayout() const noexcept;

private:
    uint32_t m_handle;
    BufferLayout m_layout;
};

#endif // !VERTEX_BUFFER_HPP