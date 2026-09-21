#include <string>
#ifdef DEBUG
#include <cstdio>
#endif

#include <glad/glad.h>

#include "program/Shader.hpp"


Shader::Shader() noexcept:
    m_handle(0),
    m_type(GL_VERTEX_SHADER)
{
    
}


Shader::Shader(Shader&& other) noexcept:
    m_handle(other.m_handle),
    m_type(other.m_type)
{
    other.m_handle = 0;
}


Shader& Shader::operator = (Shader&& other) noexcept
{
    if (this != &other)
    {
        m_handle = other.m_handle;
        m_type = other.m_type;
        other.m_handle = 0;
    }

    return *this;
}


Shader::~Shader() noexcept
{
    glDeleteShader(m_handle);
}


GLuint Shader::loadFromFile(const std::filesystem::path& filepath, GLenum shaderType) noexcept
{
    glDeleteShader(m_handle);
    m_handle = 0;

    bool type_is_valid = (shaderType == GL_COMPUTE_SHADER)         ||
                         (shaderType == GL_VERTEX_SHADER)          ||
                         (shaderType == GL_TESS_CONTROL_SHADER)    ||
                         (shaderType == GL_TESS_EVALUATION_SHADER) ||
                         (shaderType == GL_GEOMETRY_SHADER)        ||
                         (shaderType == GL_FRAGMENT_SHADER);

    if (type_is_valid)
    {
        m_type = shaderType;
        std::string source;

        if (FILE* f = fopen(filepath.string().c_str(), "r"))
        {
            fseek(f, 0, SEEK_END);
            size_t length = ftell(f);
            source.resize(length + 1);
            fseek(f, 0, SEEK_SET);
            fread(source.data(), sizeof(char), length, f);
            fclose(f);
        }

        if (!source.empty())
        {
            uint32_t shader = glCreateShader(shaderType);
            const char* src = source.c_str();

            glShaderSource(shader, 1, &src, 0);
            glCompileShader(shader);

            GLint success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (success == GL_FALSE)
            {
                GLchar infoLog[1024]{};
                glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
#ifdef DEBUG
                printf("Shader compilation status: error\n%s\n ------------------------------------------------------- \n", infoLog);
#endif
            }
            else
            {
                m_handle = shader;
            }
        }
    }

    return m_handle;
}


GLuint Shader::getHandle() const noexcept
{
    return m_handle;
}


GLenum Shader::getType() const noexcept
{
    return m_type;
}