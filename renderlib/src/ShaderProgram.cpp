#ifdef DEBUG
#include <cstdio>
#endif

#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders) noexcept:
    m_handle(0)
{
    GLuint program = glCreateProgram();

    for(const auto& shader : shaders)
        glAttachShader(program, shader.getHandle());

    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        GLchar info_log[1024]{};
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        printf("SHADER PROGRAM: Link-time error:\n%s\n", info_log);
        glDeleteProgram(program);
    }
    else
    {
        m_handle = program;
    }

    for(const auto& shader : shaders)
        glDetachShader(program, shader.getHandle());
}

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept:
    m_handle(program.m_handle)
{
    program.m_handle = 0;
}

ShaderProgram& ShaderProgram::operator =(ShaderProgram&& program) noexcept
{
    glDeleteProgram(m_handle);
    m_handle = program.m_handle;
    program.m_handle = 0;

    return *this;
}

ShaderProgram::~ShaderProgram() noexcept
{
    glDeleteProgram(m_handle);
}

void ShaderProgram::bind(bool value) noexcept
{
    GLuint program = value ? m_handle : 0;
    glUseProgram(program);
}

GLuint ShaderProgram::getHandle() const noexcept
{
    return m_handle;
}

GLint ShaderProgram::getUniformLocation(const GLchar* name) noexcept
{
    if(m_handle)
        return glGetUniformLocation(m_handle, name);

    return -1;
}

bool ShaderProgram::isCompiled() const noexcept
{
    return m_handle != 0;
}