#ifndef SHADER_HPP
#define SHADER_HPP

#include <filesystem>

#include "Export.hpp"

class RL_API Shader
{
public:
    Shader(const std::filesystem::path& filepath, uint32_t shaderType) noexcept;
    Shader(const Shader&) noexcept = delete;
    Shader(Shader&&) noexcept;
    Shader& operator =(const Shader&) noexcept = delete;
    Shader& operator =(Shader&&) noexcept;
    ~Shader() noexcept;

    uint32_t getHandle()  const noexcept;
    uint32_t getType()    const noexcept;
    bool     isCompiled() const noexcept;

private:
    uint32_t m_handle;
    uint32_t m_type;
};

#endif // !SHADER_HPP