#include <string>
#ifdef DEBUG
#include <cstdio>
#endif

#include <glad/glad.h>

#include "Shader.hpp"

Shader::Shader(const std::filesystem::path& filepath, uint32_t shaderType) noexcept:
    m_handle(0),
    m_type(0)
{
    static_assert(std::is_same_v<GLuint, uint32_t>, "GLuint and uint32_t are not the same type!");
    static_assert(std::is_same_v<GLint, int32_t>, "GLint and int32_t are not the same type!");

    bool type_is_valid = (shaderType == GL_COMPUTE_SHADER)         || 
                         (shaderType == GL_VERTEX_SHADER)          || 
                         (shaderType == GL_TESS_CONTROL_SHADER)    || 
                         (shaderType == GL_TESS_EVALUATION_SHADER) || 
                         (shaderType == GL_GEOMETRY_SHADER)        || 
                         (shaderType == GL_FRAGMENT_SHADER);

    if(type_is_valid)
    {
        m_type = shaderType;
        std::string source;

        if (FILE* f = fopen(filepath.string().c_str(), "r"); f != nullptr)
        {
            fseek(f, 0, SEEK_END);
            size_t length = ftell(f);
            source.resize(length + 1);
            fseek(f, 0, SEEK_SET);
            fread(source.data(), sizeof(char), length, f);
            fclose(f);
        }

        if(!source.empty())
        {
            uint32_t shader = glCreateShader(shaderType);
            const char* c_str = source.c_str();

            glShaderSource(shader, 1, &c_str, 0);
            glCompileShader(shader);

            int32_t success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (success == GL_FALSE)
            {
                char infoLog[1024]{};
                glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
                printf("Error: shader compilation error\n%s\n ------------------------------------------------------- \n", infoLog);
            }
            else
            {
                m_handle = shader;
            }
        }
    }
}

Shader::Shader(Shader&& tmp) noexcept:
    m_handle(tmp.m_handle),
    m_type(tmp.m_type)
{
    tmp.m_handle = 0;
}

Shader& Shader::operator =(Shader&& tmp) noexcept
{
    glDeleteShader(m_handle);
    m_handle = tmp.m_handle;
    m_type = tmp.m_type;
    tmp.m_handle = 0;

    return *this;
}

Shader::~Shader() noexcept
{
    glDeleteShader(m_handle);
}

GLuint Shader::getHandle() const noexcept
{
    return m_handle;
}

GLenum Shader::getType() const noexcept
{
    return m_type;
}

bool Shader::isCompiled() const noexcept
{
    return m_handle != 0;
}