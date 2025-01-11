#ifndef SHADER_HPP
#define SHADER_HPP

#include <filesystem>

#include "Export.hpp"

class RE_API Shader
{
public:
    enum Type
    {
        Compute,
        Vertex,
        TessControl,
        TessEvaluation,
        Geometry,
        Fragment
    };

    Shader(const std::filesystem::path& filepath, Type shaderType) noexcept;
    Shader(const Shader&) noexcept = delete;
    Shader(Shader&&) noexcept;
    Shader& operator =(const Shader&) noexcept = delete;
    Shader& operator =(Shader&&) noexcept;
    ~Shader() noexcept;

    uint32_t getHandle()  const noexcept;
    Type     getType()    const noexcept;
    bool     isCompiled() const noexcept;

private:
    uint32_t m_handle;
    Type     m_type;
};

#endif // !SHADER_HPP