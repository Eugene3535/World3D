#ifndef SHADER_MODULE_HPP
#define SHADER_MODULE_HPP

#include <vulkan/vulkan.h>

class Shader
{
public:
    explicit Shader(VkDevice device)   noexcept;
    Shader(const Shader&)              noexcept = delete;
    Shader(Shader&&)                   noexcept = delete;
    Shader& operator = (const Shader&) noexcept = delete;
    Shader& operator = (Shader&&)      noexcept = delete;
    ~Shader();

    bool loadFromFile(const char* filePath, VkShaderStageFlagBits stage) noexcept;

    VkPipelineShaderStageCreateInfo getInfo() const noexcept;

private:
    VkDevice              m_device;
    VkShaderModule        m_module;
    VkShaderStageFlagBits m_stage;
};


#endif // !SHADER_MODULE_HPP