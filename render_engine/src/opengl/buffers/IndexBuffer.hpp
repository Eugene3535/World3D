#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include "opengl/buffers/GlBuffer.hpp"

class RE_API IndexBuffer:
    public GlBuffer
{
public:
    IndexBuffer(uint32_t handle) noexcept;
    ~IndexBuffer() noexcept;
};

#endif // !INDEX_BUFFER_HPP