#ifndef GL_RESOURCE_HOLDER_HPP
#define GL_RESOURCE_HOLDER_HPP

#include <glad/glad.h>

#include "Config.hpp"
#include "opengl/resources/buffers/GlBuffer.hpp"
#include "opengl/resources/buffers/VertexArrayObject.hpp"
#include "opengl/resources/buffers/Texture2D.hpp"


class GlResourceHolder final
{
public:
    GlResourceHolder() noexcept;
    ~GlResourceHolder() noexcept;

	template<class T, size_t N>
    std::array<GLuint, N> create() noexcept;
    
    template<class T, size_t N>
    void destroy(const std::array<GLuint, N>& buffers) noexcept;

private:
    template<size_t N>
    std::array<GLuint, N> createResources(std::vector<GLuint>& handles, void(*)(int32_t, GLuint*)) noexcept;
    
    template<size_t N>
    void destroyResources(const std::array<GLuint, N>& objects, std::vector<GLuint>& handles, void(*)(int32_t, const GLuint*)) noexcept;

    std::vector<GLuint> m_buffers;
    std::vector<GLuint> m_arrays;
    std::vector<GLuint> m_textures;
};

#include "opengl/holder/GlResourceHolder.inl"

#endif // !GL_RESOURCE_HOLDER_HPP