#ifndef GL_BUFFER_HPP
#define GL_BUFFER_HPP

#include <array>
#include <vector>

#include "Export.hpp"


class RE_API GlBufferHolder final
{
public:
    GlBufferHolder() noexcept;
    GlBufferHolder(const GlBufferHolder&) noexcept = delete;
    GlBufferHolder(GlBufferHolder&&) noexcept = delete;
    GlBufferHolder& operator = (const GlBufferHolder&) noexcept = delete;
    GlBufferHolder& operator = (GlBufferHolder&&) noexcept = delete;
    ~GlBufferHolder() noexcept;

    template<size_t N>
    static std::array<uint32_t, N> generateBuffers() noexcept;
    
    template<size_t N>
    static void deleteBuffers(const std::array<uint32_t, N>& buffers) noexcept;

private:
    std::vector<uint32_t> m_buffers;
    static GlBufferHolder* m_instance;

    void(*genBuffers)(int32_t, uint32_t*);
    void(*delBuffers)(int32_t, const uint32_t*);
};

#include "gl_resources/buffers/GlBufferHolder.inl"

#endif // !GL_BUFFER_HPP