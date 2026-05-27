#include "context/Context.hpp"
#include "pipeline/descriptors/DescriptorPool.hpp"


bool DescriptorPool::create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();

    const VkDescriptorPoolCreateInfo poolInfo = 
    {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = VK_NULL_HANDLE,
        .flags         = 0,
        .maxSets       = MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes    = poolSizes.data()
    };

    bool result = (vkCreateDescriptorPool(logicalDevice, &poolInfo, VK_NULL_HANDLE, &handle) == VK_SUCCESS);

    if(result)
    {
        for(const auto& poolSize : poolSizes)
            types.push_back(poolSize.type);

        return true;
    }

    return false;
}


bool DescriptorPool::allocateDescriptorSets(std::span<VkDescriptorSet> descriptorSets, const VkDescriptorSetLayout* layouts) noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();

    const VkDescriptorSetAllocateInfo allocateInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = VK_NULL_HANDLE,
        .descriptorPool     = handle,
        .descriptorSetCount = static_cast<uint32_t>(descriptorSets.size()),
        .pSetLayouts        = layouts
    };

    return (vkAllocateDescriptorSets(logicalDevice, &allocateInfo, descriptorSets.data()) == VK_SUCCESS);
}


void DescriptorPool::writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();

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

    vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, VK_NULL_HANDLE);
}


void DescriptorPool::destroy() noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();
    vkDestroyDescriptorPool(logicalDevice, handle, VK_NULL_HANDLE);
}