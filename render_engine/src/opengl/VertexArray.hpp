#pragma once

#include "opengl/GlResource.hpp"
#include "opengl/buffers/VertexBuffer.hpp"
#include "opengl/buffers/IndexBuffer.hpp"

class RE_API VertexArray final:
    public GlResource
{
public:
    VertexArray(uint32_t handle) noexcept;
    ~VertexArray() noexcept;

    void addVertexBuffer(const VertexBuffer& vertexBuffer) noexcept;
    void setIndexBuffer(IndexBuffer& indexBuffer) noexcept;

    static void bind(VertexArray* vao) noexcept;
    uint32_t getIndexCount() const noexcept;

private:
    uint32_t m_attributeCount;
    uint32_t m_indexCount;
};