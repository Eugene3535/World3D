#include "UniformBuffer.hpp"


UniformBuffer::UniformBuffer(uint32_t handle) noexcept:
    GlBuffer(handle, GlBuffer::Target::Uniform)
{
    
}


UniformBuffer::~UniformBuffer() noexcept = default;