#include <string>
#ifdef DEBUG
#include <cstdio>
#endif

#include "Shader.hpp"

Shader::Shader(const std::filesystem::path& filepath, GLenum shaderType) noexcept:
    m_handle(0),
    m_type(0)
{
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
            GLuint shader = glCreateShader(shaderType);
            const char* c_str = source.c_str();

            glShaderSource(shader, 1, &c_str, 0);
            glCompileShader(shader);

            GLint success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (success == GL_FALSE)
            {
                char info_log[1024]{};
                glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
                printf("Error: shader compilation error\n%s\n ------------------------------------------------------- \n", info_log);
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