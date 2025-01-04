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


    static void initGlUniformFunctions() noexcept;
    static void bind(ShaderProgram* program) noexcept;
    uint32_t    getHandle() const noexcept;
    int32_t     getUniformLocation(const char* name) const noexcept;
    bool        isCompiled() const noexcept;


    static void (*setUniform1i)(int32_t, int32_t);
    static void (*setUniform2i)(int32_t, int32_t, int32_t);
    static void (*setUniform3i)(int32_t, int32_t, int32_t, int32_t);
    static void (*setUniform4i)(int32_t, int32_t, int32_t, int32_t, int32_t);

    static void (*setUniformMatrix4fv)(int32_t, int32_t, uint8_t, const float*);

private:
    uint32_t m_handle;
};

#endif // !SHADER_PROGRAM_HPP