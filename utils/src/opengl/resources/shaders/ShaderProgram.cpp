#ifdef DEBUG
#include <cstdio>
#endif

#include "opengl/resources/shaders/ShaderProgram.hpp"


ShaderProgram::ShaderProgram() noexcept:
    m_handle(0)
{
 
}


ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept:
    m_handle(other.m_handle)
{
    other.m_handle = 0;
}


ShaderProgram& ShaderProgram::operator = (ShaderProgram&& other) noexcept
{
    m_handle = other.m_handle;
    other.m_handle = 0;

    return *this;
}


ShaderProgram::~ShaderProgram() noexcept
{
    glDeleteProgram(m_handle);
}


std::optional<GLuint> ShaderProgram::link(std::span<const Shader> shaders) noexcept
{
    glDeleteProgram(m_handle);
    m_handle = 0;

    GLint program = glCreateProgram();

    for(const auto& shader : shaders)
        glAttachShader(program, shader.getHandle().value());
    
    glLinkProgram(program);

    int32_t success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        GLchar infoLog[1024]{};
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        printf("SHADER PROGRAM: Link-time error:\n%s\n", infoLog);
        glDeleteProgram(program);
    }
    else
    {
        GLuint uniformBlockIndex = glGetUniformBlockIndex(program, "Matrices");
        glUniformBlockBinding(program, uniformBlockIndex, 0);
        m_handle = program;
    }

    for(const auto& shader : shaders)
        glDetachShader(program, shader.getHandle().value());

    return getHandle();
}


std::optional<GLuint> ShaderProgram::getHandle() const noexcept
{
    if(m_handle)
        return m_handle;

    return std::nullopt;
}


GLint ShaderProgram::getUniformLocation(const char* name) const noexcept
{
    if(m_handle)
        return glGetUniformLocation(m_handle, name);

    return -1;
}