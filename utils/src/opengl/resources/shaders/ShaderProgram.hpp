#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <span>

#include "opengl/resources/shaders/Shader.hpp"

class ShaderProgram final
{
public:
    ShaderProgram() noexcept;
    ShaderProgram(const ShaderProgram&) noexcept = delete;
    ShaderProgram(ShaderProgram&&) noexcept;
    ShaderProgram& operator = (const ShaderProgram&) noexcept = delete;
    ShaderProgram& operator = (ShaderProgram&&) noexcept;
    ~ShaderProgram() noexcept;

    std::optional<GLuint> link(std::span<const Shader> shaders) noexcept;
    std::optional<GLuint> getHandle() const noexcept;
    std::optional<GLint>  getUniformLocation(const char* name) const noexcept;

private:
    GLuint m_handle;
};

#endif // !SHADER_PROGRAM_HPP