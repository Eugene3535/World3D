#ifndef SHADER_MODULE_HPP
#define SHADER_MODULE_HPP

#include <vulkan/vulkan.h>

class Shader
{
public:
    Shader()                           noexcept;
    Shader(const Shader&)              noexcept = delete;
    Shader(Shader&&)                   noexcept = delete;
    Shader& operator = (const Shader&) noexcept = delete;
    Shader& operator = (Shader&&)      noexcept = delete;
    ~Shader();

    bool loadFromFile(const char* filePath, VkShaderStageFlagBits stage, VkDevice device) noexcept;

    VkPipelineShaderStageCreateInfo getInfo() const noexcept;

private:
    VkShaderModule        m_module;
    VkShaderStageFlagBits m_stage;
    VkDevice              m_device;
};


#endif // !SHADER_MODULE_HPP