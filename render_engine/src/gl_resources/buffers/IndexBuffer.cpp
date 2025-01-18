#include "gl_resources/buffers/IndexBuffer.hpp"


IndexBuffer::IndexBuffer(uint32_t handle) noexcept:
    GlBuffer(handle, GlBuffer::Target::ElementArray)
{
    
}


IndexBuffer::~IndexBuffer() noexcept = default;