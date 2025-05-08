#include <cassert>

#include "resources/holder/GlResourceHolder.hpp"


GlResourceHolder* GlResourceHolder::s_instance;


GlResourceHolder::GlResourceHolder() noexcept
{
    assert(s_instance == nullptr);
    s_instance = this;
}


GlResourceHolder::~GlResourceHolder() noexcept
{
    if(!m_buffers.empty())
        glDeleteBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());

    if(!m_arrays.empty())
        glDeleteVertexArrays(static_cast<GLsizei>(m_arrays.size()), m_arrays.data());

    if(!m_textures.empty())
        glDeleteTextures(static_cast<GLsizei>(m_textures.size()), m_textures.data());
}


GlResourceHolder* GlResourceHolder::getInstance() noexcept
{
    return s_instance;
}


void GlResourceHolder::destroyResources(const std::span<GLuint> objects, std::vector<GLuint>& handles, void(*func)(GLint, const GLuint*)) noexcept
{
    if(!objects.empty())
    {
//      Avoid deleting non-existent objects
        std::vector<GLuint> tmp;
        tmp.reserve(objects.size());

        for(GLuint object : objects)
        {
            for(size_t i = 0; i < handles.size(); ++i)
            {
                if(auto& handle = handles[i]; handle == object)
                {
                    tmp.push_back(object);
                    std::swap(handle, handles.back());
                    handles.pop_back();
                    break;
                }
            }
        }

        if(!tmp.empty())
            func(static_cast<GLint>(tmp.size()), tmp.data());
    }
}