#ifndef GL_BUFFER_HPP
#define GL_BUFFER_HPP

#include <cstdint>

#include "Export.hpp"

class RE_API GlBuffer
{
public:
    enum class Target: uint32_t
    {
        Array,
        AtomicCounter,
        CopyRead,
        CopyWrite,
        DispatchIndirect,
        DrawIndirect,
        ElementArray,
        PixelPack,
        PixelUnpack,
        Query,
        ShaderStorage,
        Texture,
        TransformFeedback,
        Uniform
    };

    enum class Usage: uint32_t
    {
        Static,
        Dynamic,
        Stream
    };

    explicit GlBuffer(uint32_t handle, Target target) noexcept;
    virtual ~GlBuffer() noexcept;

    void create(size_t elementSize, size_t elementCount, const void* data, GlBuffer::Usage usage) noexcept;
    void update(size_t offset, size_t elementSize, size_t elementCount, const void* data) noexcept;
    bool write(const void* data, size_t size) noexcept;

    void bindBufferRange(uint32_t index, size_t offset, size_t size) noexcept;

    uint32_t getHandle() const noexcept;
    Target   getType()   const noexcept;
    uint32_t getCount()  const noexcept;

    void bind(bool enable) noexcept;

protected:
    const uint32_t m_handle;
    const Target   m_type;
    Usage          m_usage;
    uint32_t       m_count;
};

#endif // !CUSTOM_BUFFER_HPP