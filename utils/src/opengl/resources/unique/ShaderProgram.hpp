#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <span>

#include "opengl/resources/unique/Shader.hpp"

class OGL_API ShaderProgram final:
    public GlResource
{
public:
    ShaderProgram() noexcept;
    ShaderProgram(const ShaderProgram&) noexcept = delete;
    ShaderProgram(ShaderProgram&&) noexcept;
    ShaderProgram& operator = (const ShaderProgram&) noexcept = delete;
    ShaderProgram& operator = (ShaderProgram&&) noexcept;
    ~ShaderProgram() noexcept;

    void link(std::span<const Shader> shaders) noexcept;

    static void initGlUniformFunctions() noexcept;
    static void bind(ShaderProgram* program) noexcept;

    int32_t  getUniformLocation(const char* name) const noexcept;

    static void (*setUniform1i)(int32_t, int32_t);
    static void (*setUniform2i)(int32_t, int32_t, int32_t);
    static void (*setUniform3i)(int32_t, int32_t, int32_t, int32_t);
    static void (*setUniform4i)(int32_t, int32_t, int32_t, int32_t, int32_t);

    static void (*setUniformMatrix4fv)(int32_t, int32_t, uint8_t, const float*);
};

#endif // !SHADER_PROGRAM_HPP