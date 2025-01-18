#include <cassert>

#include <glad/glad.h>

#include "gl_resources/buffers/GlBufferHolder.hpp"


GlBufferHolder* GlBufferHolder::m_instance;


GlBufferHolder::GlBufferHolder() noexcept:
    genBuffers(nullptr),
    delBuffers(nullptr)
{
    assert(m_instance == nullptr);
    m_instance = this;
    genBuffers = glGenBuffers;
    delBuffers = glDeleteBuffers;
}


GlBufferHolder::~GlBufferHolder() noexcept
{
    if(!m_instance->m_buffers.empty())
        glDeleteBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());
}