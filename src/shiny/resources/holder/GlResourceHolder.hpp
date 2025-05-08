#ifndef GL_RESOURCE_HOLDER_HPP
#define GL_RESOURCE_HOLDER_HPP

#include <span>

#include <glad/glad.h>

#include "resources/buffers/GlBuffer.hpp"
#include "resources/buffers/VertexArrayObject.hpp"
#include "resources/buffers/Texture.hpp"


class SHINY_API GlResourceHolder final
{
public:
    GlResourceHolder() noexcept;
    ~GlResourceHolder() noexcept;

    static GlResourceHolder* getInstance() noexcept; 

	template<class T, size_t N>
    std::array<GLuint, N> create() noexcept;
    
    template<class T>
    void destroy(const std::span<GLuint> buffers) noexcept;

private:
    template<size_t N>
    std::array<GLuint, N> createResources(std::vector<GLuint>& handles, void(*)(int32_t, GLuint*)) noexcept;
    
    void destroyResources(const std::span<GLuint> objects, std::vector<GLuint>& handles, void(*)(int32_t, const GLuint*)) noexcept;

    std::vector<GLuint> m_buffers;
    std::vector<GLuint> m_arrays;
    std::vector<GLuint> m_textures;

    static GlResourceHolder* s_instance;
};

// typedef GlResourceHolder::getInstance() GlResources;

#include "resources/holder/GlResourceHolder.inl"

#endif // !GL_RESOURCE_HOLDER_HPP