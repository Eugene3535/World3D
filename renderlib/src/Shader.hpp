#ifndef SHADER_HPP
#define SHADER_HPP

#include <filesystem>

#include <glad/glad.h>

class Shader
{
public:
    Shader(const std::filesystem::path& filepath, GLenum shaderType) noexcept;
    Shader(const Shader&) noexcept = delete;
    Shader(Shader&&) noexcept;
    Shader& operator =(const Shader&) noexcept = delete;
    Shader& operator =(Shader&&) noexcept;
    ~Shader() noexcept;

    GLuint getHandle() const noexcept;
    GLenum getType()   const noexcept;
    bool isCompiled()  const noexcept;

private:
    GLuint m_handle;
    GLenum m_type;
};

#endif // !SHADER_HPP