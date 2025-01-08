#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include "VertexBuffer.hpp"

class RL_API IndexBuffer 
{
public:
    IndexBuffer(const void* data, size_t count, VertexBuffer::Usage usage = VertexBuffer::Usage::Static);
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&& index_buffer) noexcept;
    IndexBuffer& operator = (const IndexBuffer&) = delete;
    IndexBuffer& operator = (IndexBuffer&& index_buffer) noexcept;
    ~IndexBuffer() noexcept;

    static void bind(IndexBuffer* buffer) noexcept;
    uint32_t getCount() const noexcept;

private:
    uint32_t m_handle;
    uint32_t m_count;
};

#endif // !INDEX_BUFFER_HPP