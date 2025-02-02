#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include "opengl/resources/buffers/GlBuffer.hpp"

class OGL_API IndexBuffer:
    public GlBuffer
{
public:
    explicit IndexBuffer(uint32_t handle) noexcept;
    ~IndexBuffer() noexcept;
};

#endif // !INDEX_BUFFER_HPP