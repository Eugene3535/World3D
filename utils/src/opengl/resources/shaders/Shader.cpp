#include <string>
#ifdef DEBUG
#include <cstdio>
#endif

#include <glad/glad.h>

#include "opengl/resources/shaders/Shader.hpp"


Shader::Shader() noexcept:
    GlResource(0),
    m_type(Shader::Type::Vertex)
{
    
}


Shader::~Shader() noexcept
{
    glDeleteShader(m_handle);
}


void Shader::loadFromFile(const std::filesystem::path& filepath, Shader::Type shaderType) noexcept
{
    glDeleteShader(m_handle);
    m_handle = 0;

    bool type_is_valid = (shaderType == Shader::Compute)        ||
                         (shaderType == Shader::Vertex)         ||
                         (shaderType == Shader::TessControl)    ||
                         (shaderType == Shader::TessEvaluation) ||
                         (shaderType == Shader::Geometry)       ||
                         (shaderType == Shader::Fragment);

    auto ShaderTypeToGlType = [](Shader::Type type)
    {
        switch (type)
        {
            case Shader::Compute:        return GL_COMPUTE_SHADER;
            case Shader::Vertex:         return GL_VERTEX_SHADER;
            case Shader::TessControl:    return GL_TESS_CONTROL_SHADER;
            case Shader::TessEvaluation: return GL_TESS_EVALUATION_SHADER;
            case Shader::Geometry:       return GL_GEOMETRY_SHADER;
            case Shader::Fragment:       return GL_FRAGMENT_SHADER;

            default: return GL_VERTEX_SHADER;
        }
    };

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
            uint32_t shader = glCreateShader(ShaderTypeToGlType(shaderType));
            const char* c_str = source.c_str();

            glShaderSource(shader, 1, &c_str, 0);
            glCompileShader(shader);

            int32_t success = 0;
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


Shader::Type Shader::getType() const noexcept
{
    return m_type;
}