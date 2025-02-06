#ifndef VERTEX_ARRAY_OBJECT_HPP
#define VERTEX_ARRAY_OBJECT_HPP

#include "opengl/resources/buffers/GlBuffer.hpp"
#include "opengl/resources/buffers/VertexBufferLayout.hpp"


class OGL_API VertexArrayObject final
{
public:
    explicit VertexArrayObject(GLuint handle) noexcept;

    void addVertexBuffer(const GlBuffer& buffer, const VertexBufferLayout& layout) noexcept;
    void setElementBuffer(const GlBuffer& buffer) noexcept;

    GLuint getHandle()     const noexcept;
    GLuint getIndexCount() const noexcept;

private:
    const GLuint m_handle;
    GLuint m_attributeCount;
    GLuint m_indexCount;
};

#endif // !VERTEX_ARRAY_OBJECT_HPP