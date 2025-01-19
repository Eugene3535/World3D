#ifndef SHADER_HPP
#define SHADER_HPP

#include <filesystem>

#include "opengl/GlResource.hpp"

class RE_API Shader final:
    public GlResource
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
    Shader(Shader&&) noexcept = delete;
    Shader& operator = (const Shader&) noexcept = delete;
    Shader& operator = (Shader&&) noexcept = delete;
    ~Shader() noexcept;

    Type getType() const noexcept;

private:
    Type m_type;
};

#endif // !SHADER_HPP