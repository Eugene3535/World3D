#ifndef SHADER_HPP
#define SHADER_HPP

#include <filesystem>
#include <optional>

#include <glad/glad.h>

#include "Export.hpp"

class OGL_API Shader final
{
public:
    Shader() noexcept;
    Shader(const Shader&) noexcept = delete;
    Shader(Shader&&) noexcept = delete;
    Shader& operator = (const Shader&) noexcept = delete;
    Shader& operator = (Shader&&) noexcept = delete;
    ~Shader() noexcept;

    std::optional<GLuint> loadFromFile(const std::filesystem::path& filepath, GLenum shaderType) noexcept;
    std::optional<GLuint> getHandle() const noexcept;
    GLenum                getType()   const noexcept;

private:
    GLuint m_handle;
    GLenum m_type;
};

#endif // !SHADER_HPP