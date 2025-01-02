#ifdef DEBUG
#include <cstdio>
#endif

#include <glad/glad.h>

#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders) noexcept:
    m_handle(0)
{
    static_assert(std::is_same_v<GLchar, char>, "GLchar and char are not the same type!");
    static_assert(std::is_same_v<GLuint, uint32_t>, "GLuint and uint32_t are not the same type!");
    static_assert(std::is_same_v<GLint, int32_t>, "GLint and int32_t are not the same type!");

    uint32_t program = glCreateProgram();

    for(const auto& shader : shaders)
        glAttachShader(program, shader.getHandle());

    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        char infoLog[1024]{};
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        printf("SHADER PROGRAM: Link-time error:\n%s\n", infoLog);
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

void ShaderProgram::bind(ShaderProgram* program) noexcept
{
    uint32_t handle = program ? program->m_handle : 0;
    glUseProgram(handle);
}

uint32_t ShaderProgram::getHandle() const noexcept
{
    return m_handle;
}

int32_t ShaderProgram::getUniformLocation(const char* name) const noexcept
{
    if(m_handle)
        return glGetUniformLocation(m_handle, name);

    return -1;
}

bool ShaderProgram::isCompiled() const noexcept
{
    return m_handle != 0;
}