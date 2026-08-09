#pragma once

#include <filesystem>

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

    bool loadFromFile(const std::filesystem::path& filePath, VkShaderStageFlagBits stage) noexcept;

    VkPipelineShaderStageCreateInfo getInfo() const noexcept;

private:
    VkDevice              m_device;
    VkShaderModule        m_module;
    VkShaderStageFlagBits m_stage;
};