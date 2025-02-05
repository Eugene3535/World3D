#ifndef GL_RESOURCE_HOLDER_HPP
#define GL_RESOURCE_HOLDER_HPP

#include <glad/glad.h>

#include "opengl/resources/buffers/GlBuffer.hpp"
#include "opengl/resources/graphics/VertexArrayObject.hpp"
#include "opengl/resources/graphics/Texture2D.hpp"

class OGL_API GlResourceHolder final
{
public:
    GlResourceHolder() noexcept;
    ~GlResourceHolder() noexcept;

    bool isLoaded() noexcept;

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

    bool m_isLoaded;
};

#include "opengl/holder/GlResourceHolder.inl"

#endif //  !GL_RESOURCE_HOLDER_HPP