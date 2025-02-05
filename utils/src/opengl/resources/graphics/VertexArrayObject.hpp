#pragma once

#include "opengl/resources/GlResource.hpp"
#include "opengl/resources/buffers/VertexBuffer.hpp"
#include "opengl/resources/buffers/IndexBuffer.hpp"


enum class PrimitiveType
{
    Points, 
    LineStrip,
    LineLoop, 
    Lines, 
    LineStripAdjacency, 
    LinesAdjacency, 
    TriangleStrip, 
    TriangleFan, 
    Triangles, 
    TriangleStripAdjacency, 
    TrianglesAdjacency, 
    Patches
};


class OGL_API VertexArrayObject final:
    public GlResource
{
public:
    VertexArrayObject(uint32_t handle) noexcept;
    ~VertexArrayObject() noexcept;

    void addVertexBuffer(const VertexBuffer& vertexBuffer) noexcept;
    void setIndexBuffer(IndexBuffer& indexBuffer) noexcept;

    static void bind(VertexArrayObject* vao) noexcept;
    uint32_t getIndexCount() const noexcept;

    void drawArrays(PrimitiveType mode, int32_t first, int32_t count) noexcept;
    void drawElements(PrimitiveType mode, int32_t count, const void* indices = nullptr) noexcept;

private:
    uint32_t m_attributeCount;
    uint32_t m_indexCount;
};