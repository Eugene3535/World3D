#ifndef VERTEX_BUFFER_LAYOUT_HPP
#define VERTEX_BUFFER_LAYOUT_HPP

#include <span>
#include <vector>

#include <glad/glad.h>

#include "Export.hpp"

// A vertex attribute is an input variable to a shader that is supplied with per - vertex data.
// In OpenGL core profile, they are specified as in variables in a vertex shader and are backed by a GL_ARRAY_BUFFER.
// These variable can contain, for example, positions, normals or texture coordinates.
// A vertex is the combination of all the vertex attributes that belong together.


class OGL_API VertexBufferLayout
{
public:
    class OGL_API Attribute
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

        Attribute(Type type) noexcept;

        Type   type;
        GLuint componentType;
        size_t componentsCount;
        size_t sizeInBytes;
        size_t offset;
    };

    VertexBufferLayout(std::span<const Attribute> attributes) noexcept;

    std::span<const Attribute> getAttributes() const noexcept;
    size_t getStride() const noexcept;

private:
    std::vector<Attribute> m_attributes;
    size_t m_stride;
};

#endif // !VERTEX_BUFFER_LAYOUT_HPP