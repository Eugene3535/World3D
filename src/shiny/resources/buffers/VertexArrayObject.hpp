#ifndef VERTEX_ARRAY_OBJECT_HPP
#define VERTEX_ARRAY_OBJECT_HPP

#include "Export.hpp"
#include "resources/buffers/GlBuffer.hpp"
#include "resources/buffers/VertexBufferLayout.hpp"


class SHINY_API VertexArrayObject final
{
public:
    VertexArrayObject() noexcept;
    VertexArrayObject(GLuint handle) noexcept;

    void addVertexBuffer(const GlBuffer& buffer, std::span<const VertexBufferLayout::Attribute> attributes) noexcept;
    void setElementBuffer(const GlBuffer& buffer) noexcept;

    GLuint getHandle()     const noexcept;
    GLuint getIndexCount() const noexcept;

private:
    GLuint m_handle;
    GLuint m_attributeCount;
    GLuint m_indexCount;
};

#endif // !VERTEX_ARRAY_OBJECT_HPP