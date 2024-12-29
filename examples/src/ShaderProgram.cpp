#include <iostream>

#include <glad/glad.h>

#include "ShaderProgram.hpp"

uint32_t ShaderProgram::compile(const std::filesystem::path& vert, const std::filesystem::path& frag) noexcept
{
    uint32_t program = glCreateProgram();

    std::string vertSrc = readShaderSourceFromFile(vert);
    std::string fragSrc = readShaderSourceFromFile(frag);

    uint32_t vertShader = compileShaderFromSource(vertSrc, GL_VERTEX_SHADER);
    uint32_t fragShader = compileShaderFromSource(fragSrc, GL_FRAGMENT_SHADER);

    linkToProgram(program, vertShader);
    linkToProgram(program, fragShader);

    return program;
}

std::string ShaderProgram::readShaderSourceFromFile(const std::filesystem::path& fPath) noexcept
{
    std::string source;

    if (FILE* f = fopen(fPath.generic_string().c_str(), "r"); f != nullptr)
    {
        fseek(f, 0, SEEK_END);
        size_t length = ftell(f);

        source.resize(length + 1);

        fseek(f, 0, SEEK_SET);
        fread(source.data(), sizeof(char), length, f);
        fclose(f);
    }

    return source;
}

uint32_t ShaderProgram::compileShaderFromSource(const std::string& source, uint32_t type) noexcept
{
    uint32_t shader = glCreateShader(type);
    const char* c_str = source.c_str();

    glShaderSource(shader, 1, &c_str, 0);
    glCompileShader(shader);

    int success = 0;
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[1024]{};

        glGetShaderInfoLog(shader, 1024, NULL, infoLog);

        std::cerr << "Error: shader compilation error\n"
            << infoLog << "\n -- --------------------------------------------------- -- \n";

        return 0;
    }

    return shader;
}

void ShaderProgram::linkToProgram(uint32_t program, uint32_t shader) noexcept
{
    glAttachShader(program, shader);
    glLinkProgram(program); 

    int32_t success = 0;
    char infoLog[1024]{};
  
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);

        std::cerr << "Error: shader program link error\n"
            << infoLog << "\n -- --------------------------------------------------- -- \n";
    }

    glDeleteShader(shader);
}