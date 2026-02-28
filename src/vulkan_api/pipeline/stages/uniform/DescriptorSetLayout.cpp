#include <stdlib.h>
#include <string.h>

#include "pipeline/stages/uniform/DescriptorSetLayout.hpp"


void DescriptorSetLayout::addDescriptor(VkDescriptorType type, VkShaderStageFlagBits shaderStage) noexcept
{
    const uint32_t binding = static_cast<uint32_t>(m_bindings.size());
    const VkShaderStageFlags flags = shaderStage;

    const VkDescriptorSetLayoutBinding nextBinding = 
    {
        .binding            = binding,
        .descriptorType     = type,
        .descriptorCount    = 1,
        .stageFlags         = flags,
        .pImmutableSamplers = VK_NULL_HANDLE
    };

    m_bindings.push_back(nextBinding);
}


VkDescriptorSetLayoutCreateInfo DescriptorSetLayout::getInfo() const noexcept
{
    const VkDescriptorSetLayoutCreateInfo info = 
    {
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = VK_NULL_HANDLE,
        .flags        = 0,
        .bindingCount = static_cast<uint32_t>(m_bindings.size()),
        .pBindings    = m_bindings.data()
    };

    return info;
}
