#include "opengl/holder/GlResourceHolder.hpp"


GlResourceHolder::GlResourceHolder() noexcept
{

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