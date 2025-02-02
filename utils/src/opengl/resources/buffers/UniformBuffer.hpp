#ifndef UNIFORM_BUFFER_HPP
#define UNIFORM_BUFFER_HPP

#include "opengl/resources/buffers/GlBuffer.hpp"


class OGL_API UniformBuffer:
    public GlBuffer
{
public:
    explicit UniformBuffer(uint32_t handle) noexcept;
    ~UniformBuffer() noexcept;
};

#endif // !UNIFORM_BUFFER_HPP