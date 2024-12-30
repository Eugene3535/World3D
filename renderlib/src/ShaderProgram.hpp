#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include "Shader.hpp"

class RL_API ShaderProgram
{
public:
    ShaderProgram(std::initializer_list<Shader> shaders) noexcept;
    ShaderProgram(const ShaderProgram&) noexcept = delete;
    ShaderProgram(ShaderProgram&&) noexcept;
    ShaderProgram& operator =(const ShaderProgram&) noexcept = delete;
    ShaderProgram& operator =(ShaderProgram&&) noexcept;
    ~ShaderProgram() noexcept;

    void   bind(bool value) noexcept;
    GLuint getHandle() const noexcept;
    GLint  getUniformLocation(const GLchar* name) noexcept;
    bool   isCompiled() const noexcept;

private:
    GLuint m_handle;
};

#endif // !SHADER_PROGRAM_HPP