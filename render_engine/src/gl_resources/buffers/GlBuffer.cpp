#include <cstring>

#include <glad/glad.h>

#include "gl_resources/buffers/GlBuffer.hpp"


constexpr static GLenum targetToGlEnum(const GlBuffer::Target target) noexcept
{
    switch (target)
    {
        case GlBuffer::Target::Array:             return GL_ARRAY_BUFFER;
        case GlBuffer::Target::AtomicCounter:     return GL_ATOMIC_COUNTER_BUFFER;
        case GlBuffer::Target::CopyRead:          return GL_COPY_READ_BUFFER;
        case GlBuffer::Target::CopyWrite:         return GL_COPY_WRITE_BUFFER;
        case GlBuffer::Target::DispatchIndirect:  return GL_DISPATCH_INDIRECT_BUFFER;
        case GlBuffer::Target::DrawIndirect:      return GL_DRAW_INDIRECT_BUFFER;
        case GlBuffer::Target::ElementArray:      return GL_ELEMENT_ARRAY_BUFFER;
        case GlBuffer::Target::PixelPack:         return GL_PIXEL_PACK_BUFFER;
        case GlBuffer::Target::PixelUnpack:       return GL_PIXEL_UNPACK_BUFFER;
        case GlBuffer::Target::Query:             return GL_QUERY_BUFFER;
        case GlBuffer::Target::ShaderStorage:     return GL_TEXTURE_BUFFER;
        case GlBuffer::Target::Texture:           return GL_SHADER_STORAGE_BUFFER;
        case GlBuffer::Target::TransformFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
        case GlBuffer::Target::Uniform:           return GL_UNIFORM_BUFFER;    
    
        default: return GL_ARRAY_BUFFER;
    }
};


constexpr static GLenum usageToGlEnum(const GlBuffer::Usage usage) noexcept
{
    switch (usage)
    {
        case GlBuffer::Usage::Static:  return GL_STATIC_DRAW;
        case GlBuffer::Usage::Dynamic: return GL_DYNAMIC_DRAW;
        case GlBuffer::Usage::Stream:  return GL_STREAM_DRAW;

        default: return GL_STREAM_DRAW;
    }
};


GlBuffer::GlBuffer(uint32_t handle, Target target) noexcept:
    m_handle(handle),
    m_type(target),
    m_usage(GlBuffer::Usage::Stream),
    m_count(0)
{

}


GlBuffer::~GlBuffer() noexcept = default;


void GlBuffer::create(const void* data, size_t elementCount, size_t elementSize, GlBuffer::Usage usage) noexcept
{
    if(data)
    {
        glBindBuffer(targetToGlEnum(m_type), m_handle);
        glBufferData(targetToGlEnum(m_type), static_cast<GLsizeiptr>(elementCount * elementSize), data, usageToGlEnum(usage));
        glBindBuffer(targetToGlEnum(m_type), 0);

        m_usage = usage;
        m_count = static_cast<uint32_t>(elementCount);
    }
}


void GlBuffer::update(const void* data, size_t elementCount, size_t elementSize, size_t offset) noexcept
{
    if(data)
    {
        glBindBuffer(targetToGlEnum(m_type), m_handle);

        if(elementCount >= m_count)
        {
            glBufferData(targetToGlEnum(m_type), static_cast<GLsizeiptr>(elementCount * elementSize), nullptr, usageToGlEnum(m_usage));
            m_count = static_cast<uint32_t>(elementCount);
        }
            
        glBufferSubData(targetToGlEnum(m_type), static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(elementCount * elementSize), data);
        glBindBuffer(targetToGlEnum(m_type), 0);
    }
}


bool GlBuffer::write(const void* data, size_t size, size_t offset) noexcept
{
    glBindBuffer(targetToGlEnum(m_type), m_handle);

    void* ptr = glMapBuffer(targetToGlEnum(m_type), GL_WRITE_ONLY);
    memcpy(ptr + offset, data, size);
    GLboolean result = glUnmapBuffer(targetToGlEnum(m_type));

    glBindBuffer(targetToGlEnum(m_type), 0);

    return (result == GL_TRUE);
}


uint32_t GlBuffer::getHandle() const noexcept
{
    return m_handle;
}


GlBuffer::Target GlBuffer::getType() const noexcept
{
    return m_type;
}


uint32_t GlBuffer::getCount() const noexcept
{
    return m_count;
}


void GlBuffer::bind(bool enable) noexcept
{
    glBindBuffer(targetToGlEnum(m_type), enable ? m_handle : 0);
}