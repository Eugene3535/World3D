#ifndef DESCRIPTOR_SET_LAYOUT_HPP
#define DESCRIPTOR_SET_LAYOUT_HPP

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

struct DescriptorSetLayout
{
public:
    void addDescriptor(VkDescriptorType type, VkShaderStageFlagBits shaderStage) noexcept;
    VkDescriptorSetLayoutCreateInfo getInfo() const noexcept;

private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
};

#endif // !DESCRIPTOR_SET_LAYOUT_HPP