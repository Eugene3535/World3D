#ifndef UNIFORM_BUFFER_HPP
#define UNIFORM_BUFFER_HPP

#include "opengl/resources/buffers/GlBuffer.hpp"


class RE_API UniformBuffer:
    public GlBuffer
{
public:
    UniformBuffer(uint32_t handle) noexcept;
    ~UniformBuffer() noexcept;
};

#endif // !UNIFORM_BUFFER_HPP