#include "context/Context.hpp"
#include "pipeline/descriptors/DescriptorPool.hpp"


bool DescriptorPool::create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept
{
    const VkDescriptorPoolCreateInfo poolInfo = 
    {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = VK_NULL_HANDLE,
        .flags         = 0,
        .maxSets       = MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes    = poolSizes.data()
    };

    return (vkCreateDescriptorPool(vkContext->getLogicalDevice()->getHandle(), &poolInfo, VK_NULL_HANDLE, &m_handle) == VK_SUCCESS);
}


void DescriptorPool::destroy() noexcept
{
    auto device = vkContext->getLogicalDevice()->getHandle();
    vkDestroyDescriptorPool(device, m_handle, VK_NULL_HANDLE);
}


bool DescriptorPool::allocateDescriptorSets(std::span<VkDescriptorSet> descriptorSets, const VkDescriptorSetLayout* layouts) noexcept
{
    const VkDescriptorSetAllocateInfo allocateInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = VK_NULL_HANDLE,
        .descriptorPool     = m_handle,
        .descriptorSetCount = static_cast<uint32_t>(descriptorSets.size()),
        .pSetLayouts        = layouts
    };

    return (vkAllocateDescriptorSets(vkContext->getLogicalDevice()->getHandle(), &allocateInfo, descriptorSets.data()) == VK_SUCCESS);
}


void DescriptorPool::writeBufferInfo(const VkDescriptorBufferInfo* bufferInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept
{
    const VkWriteDescriptorSet descriptorWrite = 
    {
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = VK_NULL_HANDLE,
        .dstSet           = descriptorSet,
        .dstBinding       = dstBinding,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = VK_NULL_HANDLE,
        .pBufferInfo      = bufferInfo,
        .pTexelBufferView = VK_NULL_HANDLE
    };

    vkUpdateDescriptorSets(vkContext->getLogicalDevice()->getHandle(), 1, &descriptorWrite, 0, VK_NULL_HANDLE);
}


void DescriptorPool::writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept
{
    const VkWriteDescriptorSet descriptorWrite = 
    {
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = VK_NULL_HANDLE,
        .dstSet           = descriptorSet,
        .dstBinding       = dstBinding,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = imageInfo,
        .pBufferInfo      = VK_NULL_HANDLE,
        .pTexelBufferView = VK_NULL_HANDLE
    };

    vkUpdateDescriptorSets(vkContext->getLogicalDevice()->getHandle(), 1, &descriptorWrite, 0, VK_NULL_HANDLE);
}