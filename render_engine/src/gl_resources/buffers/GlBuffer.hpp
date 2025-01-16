#ifndef GL_BUFFER_HPP
#define GL_BUFFER_HPP

#include <vector>

class GlBuffer
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
    ~GlBuffer() noexcept;

    static void bind(Target target, uint32_t buffer) noexcept;

private:
    static std::vector<uint32_t> m_buffers;
    static GlBuffer* s_instance;
};

#endif // !GL_BUFFER_HPP