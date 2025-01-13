#ifdef DEBUG
#include <cstdio>
#endif

#include <glad/glad.h>

#include "ShaderProgram.hpp"


void (*ShaderProgram::setUniform1i)(int32_t, int32_t);
void (*ShaderProgram::setUniform2i)(int32_t, int32_t, int32_t);
void (*ShaderProgram::setUniform3i)(int32_t, int32_t, int32_t, int32_t);
void (*ShaderProgram::setUniform4i)(int32_t, int32_t, int32_t, int32_t, int32_t);

void (*ShaderProgram::setUniformMatrix4fv)(int32_t location, int32_t count, uint8_t transpose, const float* value);


ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders) noexcept:
    m_handle(0)
{
    uint32_t program = glCreateProgram();

    for(const auto& shader : shaders)
        glAttachShader(program, shader.getHandle());

    glLinkProgram(program);

    int32_t success = GL_FALSE;
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
        uint32_t uniformBlockIndex = glGetUniformBlockIndex(program, "Matrix");
        glUniformBlockBinding(program, uniformBlockIndex, 0);
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


void ShaderProgram::initGlUniformFunctions() noexcept
{
    setUniform1i = glUniform1i;
    setUniform2i = glUniform2i;
    setUniform3i = glUniform3i;
    setUniform4i = glUniform4i;

    setUniformMatrix4fv = glUniformMatrix4fv;
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