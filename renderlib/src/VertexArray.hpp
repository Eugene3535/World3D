#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

class VertexArray 
{
public:
    VertexArray() noexcept;
    VertexArray(const VertexArray&) noexcept = delete;
    VertexArray(VertexArray&& vertexBuffer) noexcept;
    VertexArray& operator=(const VertexArray&) noexcept = delete;
    VertexArray& operator=(VertexArray&& vertexBuffer) noexcept;
    ~VertexArray() noexcept;

    void addVertexBuffer(const VertexBuffer& vertexBuffer) noexcept;
    void setIndexBuffer(IndexBuffer& indexBuffer) noexcept;

    static void bind(VertexArray* vao) noexcept;
    uint32_t getIndexCount() const noexcept;

private:
    uint32_t m_handle;
    uint32_t m_attributeCount;
    uint32_t m_indexCount;
};