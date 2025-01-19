#ifndef GL_RESOURCE_HOLDER_HPP
#define GL_RESOURCE_HOLDER_HPP

#include <array>
#include <vector>

#include "opengl/resources/buffers/GlBuffer.hpp"
#include "opengl/resources/shared/VertexArray.hpp"
#include "opengl/resources/shared/Texture2D.hpp"

class RE_API GlResourceHolder final
{
public:
    GlResourceHolder() noexcept;
    GlResourceHolder(const GlResourceHolder&) noexcept = delete;
    GlResourceHolder(GlResourceHolder&&) noexcept = delete;
    GlResourceHolder& operator = (const GlResourceHolder&) noexcept = delete;
    GlResourceHolder& operator = (GlResourceHolder&&) noexcept = delete;
    ~GlResourceHolder() noexcept;

    template<class T, size_t N>
    std::array<uint32_t, N> create() noexcept;
    
    template<class T, size_t N>
    void destroy(const std::array<uint32_t, N>& buffers) noexcept;

private:
    template<size_t N>
    std::array<uint32_t, N> createResources(std::vector<uint32_t>& handles, void(*)(int32_t, uint32_t*)) noexcept;
    
    template<size_t N>
    void destroyResources(const std::array<uint32_t, N>& objects, std::vector<uint32_t>& handles, void(*)(int32_t, const uint32_t*)) noexcept;

    std::vector<uint32_t> m_buffers;
    std::vector<uint32_t> m_arrays;
    std::vector<uint32_t> m_textures;

    static GlResourceHolder* m_instance;

    void(*genBuffers)(int32_t, uint32_t*);
    void(*delBuffers)(int32_t, const uint32_t*);

    void(*genVertexArrays)(int32_t, uint32_t*);
    void(*delVertexArrays)(int32_t, const uint32_t*);

    void(*genTextures)(int32_t, uint32_t*);
    void(*delTextures)(int32_t, const uint32_t*);
};

#include "opengl/resources/GlResourceHolder.inl"

#endif // !GL_RESOURCE_HOLDER_HPP