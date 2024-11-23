#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <filesystem>

class Shader
{
public:
    Shader() noexcept;
    ~Shader();
    
    bool compile(const std::filesystem::path& filename, unsigned type) noexcept;
    bool compile(const std::filesystem::path& filenameVert, const std::filesystem::path& filenameFrag) noexcept;
    bool compile(const std::filesystem::path& filenameVert, const std::filesystem::path& filenameFrag, const std::filesystem::path& filenameGeom) noexcept;

    int getUniformLocation(const char* name) const noexcept;

    static void bind(const Shader* shader) noexcept;

private:
    std::string readShaderSourceFromFile(const std::filesystem::path& fPath);
    unsigned    compileShaderFromSource(const std::string& source, unsigned type);
    bool        linkToProgram(unsigned shader);

private:
    unsigned m_program;
};
#endif