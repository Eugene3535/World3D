#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include "opengl/resources/unique/Shader.hpp"

class RE_API ShaderProgram final:
    public GlResource
{
public:
    ShaderProgram(std::initializer_list<Shader> shaders) noexcept;
    ShaderProgram(ShaderProgram&&) noexcept;
    ShaderProgram& operator = (ShaderProgram&&) noexcept;
    ~ShaderProgram() noexcept;

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