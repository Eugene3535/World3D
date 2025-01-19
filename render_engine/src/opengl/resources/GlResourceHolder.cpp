#include <cassert>

#include <glad/glad.h>

#include "opengl/resources/GlResourceHolder.hpp"


GlResourceHolder* GlResourceHolder::m_instance;


GlResourceHolder::GlResourceHolder() noexcept:
    genBuffers(nullptr),
    delBuffers(nullptr),
    genVertexArrays(nullptr),
    delVertexArrays(nullptr)
{
    assert(m_instance == nullptr);
    m_instance = this;

    genBuffers = glGenBuffers;
    delBuffers = glDeleteBuffers;

    genVertexArrays = glGenVertexArrays;
    delVertexArrays = glDeleteVertexArrays;
}


GlResourceHolder::~GlResourceHolder() noexcept
{
    if(!m_buffers.empty())
        glDeleteBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());

    if(!m_arrays.empty())
        glDeleteVertexArrays(static_cast<GLsizei>(m_arrays.size()), m_arrays.data());
}