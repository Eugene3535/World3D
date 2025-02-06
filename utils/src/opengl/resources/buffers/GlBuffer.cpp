#include <cstring>

#include "opengl/resources/buffers/GlBuffer.hpp"


GlBuffer::GlBuffer(GLuint handle, GLenum target) noexcept:
    m_handle(handle),
    m_target(target),
    m_usage(GL_STREAM_DRAW),
    m_count(0)
{
    
}


void GlBuffer::create(size_t elementSize, size_t elementCount, const void* data, GLenum usage) noexcept
{
    glBindBuffer(m_target, m_handle);
    glBufferData(m_target, static_cast<GLsizeiptr>(elementCount * elementSize), data, usage);
    glBindBuffer(m_target, 0);

    m_usage = usage;
    m_count = static_cast<GLuint>(elementCount);
}


void GlBuffer::update(size_t offset, size_t elementSize, size_t elementCount, const void* data) noexcept
{
    glBindBuffer(m_target, m_handle);

    if(elementCount > m_count)
    {
        glBufferData(m_target, static_cast<GLsizeiptr>(elementCount * elementSize), nullptr, m_usage);
        m_count = static_cast<GLuint>(elementCount);
    }
        
    glBufferSubData(m_target, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(elementCount * elementSize), data);
    glBindBuffer(m_target, 0);
}


void GlBuffer::bindBufferRange(GLuint index, size_t offset, size_t size) noexcept
{
    glBindBuffer(m_target, m_handle);
//  target must be one of GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER, or GL_SHADER_STORAGE_BUFFER.
    const bool target_is_correct = ((m_target == GL_ATOMIC_COUNTER_BUFFER)     ||
                                    (m_target == GL_TRANSFORM_FEEDBACK_BUFFER) ||
                                    (m_target == GL_UNIFORM_BUFFER)            ||
                                    (m_target == GL_SHADER_STORAGE_BUFFER));                   

    if(target_is_correct)
        glBindBufferRange(m_target, index, m_handle, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));

    glBindBuffer(m_target, 0);
}


GLuint GlBuffer::getHandle() const noexcept
{
    return m_handle;
}


GLenum GlBuffer::getTarget() const noexcept
{
    return m_target;
}


GLuint GlBuffer::getCount() const noexcept
{
    return m_count;
}