#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class DescriptorSetLayout
{
public:
    void addDescriptor(VkDescriptorType type, VkShaderStageFlagBits shaderStage) noexcept;
    VkDescriptorSetLayoutCreateInfo getInfo() const noexcept;

private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
};