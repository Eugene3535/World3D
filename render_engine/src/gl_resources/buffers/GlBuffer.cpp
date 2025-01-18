#include <cassert>

#include <glad/glad.h>

#include "gl_resources/buffers/GlBuffer.hpp"


GlBuffer* GlBuffer::m_instance;


GlBuffer::GlBuffer() noexcept:
    genBuffers(nullptr),
    delBuffers(nullptr)
{
    assert(m_instance == nullptr);
    m_instance = this;
    genBuffers = glGenBuffers;
    delBuffers = glDeleteBuffers;
}


GlBuffer::~GlBuffer() noexcept
{
    if(!m_instance->m_buffers.empty())
        glDeleteBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());
}


void GlBuffer::bind(GlBuffer::Target target, uint32_t buffer) noexcept
{
    auto targetToGlenum = [](GlBuffer::Target target) noexcept -> GLenum
    {
        switch (target)
        {
            case GlBuffer::Array:             return GL_ARRAY_BUFFER;
            case GlBuffer::AtomicCounter:     return GL_ATOMIC_COUNTER_BUFFER;
            case GlBuffer::CopyRead:          return GL_COPY_READ_BUFFER;
            case GlBuffer::CopyWrite:         return GL_COPY_WRITE_BUFFER;
            case GlBuffer::DispatchIndirect:  return GL_DISPATCH_INDIRECT_BUFFER;
            case GlBuffer::DrawIndirect:      return GL_DRAW_INDIRECT_BUFFER;
            case GlBuffer::ElementArray:      return GL_ELEMENT_ARRAY_BUFFER;
            case GlBuffer::PixelPack:         return GL_PIXEL_PACK_BUFFER;
            case GlBuffer::PixelUnpack:       return GL_PIXEL_UNPACK_BUFFER;
            case GlBuffer::Query:             return GL_QUERY_BUFFER;
            case GlBuffer::ShaderStorage:     return GL_TEXTURE_BUFFER;
            case GlBuffer::Texture:           return GL_SHADER_STORAGE_BUFFER;
            case GlBuffer::TransformFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
            case GlBuffer::Uniform:           return GL_UNIFORM_BUFFER;    
        
            default: return GL_ARRAY_BUFFER;
        }
    };

    assert(m_instance);
    glBindBuffer(targetToGlenum(target), buffer);
}