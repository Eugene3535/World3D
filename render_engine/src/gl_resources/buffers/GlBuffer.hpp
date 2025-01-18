#ifndef GL_BUFFER_HPP
#define GL_BUFFER_HPP

#include <array>
#include <vector>

#include "Export.hpp"


class RE_API GlBuffer final
{
public:
    enum Target: uint32_t
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

    enum Usage: uint32_t
    {
        Static,
        Dynamic,
        Stream
    };

    GlBuffer() noexcept;
    GlBuffer(const GlBuffer&) noexcept = delete;
    GlBuffer(GlBuffer&&) noexcept = delete;
    GlBuffer& operator = (const GlBuffer&) noexcept = delete;
    GlBuffer& operator = (GlBuffer&&) noexcept = delete;
    ~GlBuffer() noexcept;

    template<size_t N>
    static void deleteBuffers(const std::array<uint32_t, N>& buffers) noexcept;

    template<size_t N>
    static std::array<uint32_t, N> generateBuffers() noexcept;

    static void bind(Target target, uint32_t buffer) noexcept;

private:
    std::vector<uint32_t> m_buffers;
    static GlBuffer* m_instance;

    void(*genBuffers)(int32_t, uint32_t*);
    void(*delBuffers)(int32_t, const uint32_t*);
};

#include "gl_resources/buffers/GlBuffer.inl"

#endif // !GL_BUFFER_HPP