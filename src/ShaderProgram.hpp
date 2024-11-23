#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <string>
#include <filesystem>

class ShaderProgram
{
public:
    uint32_t compile(const std::filesystem::path& vert, const std::filesystem::path& frag) noexcept;

private:
    std::string readShaderSourceFromFile(const std::filesystem::path& fPath) noexcept;
    uint32_t    compileShaderFromSource(const std::string& source, uint32_t type) noexcept;
    void        linkToProgram(uint32_t program, uint32_t shader) noexcept;
};

#endif // !SHADER_PROGRAM_HPP