#include "pipeline/descriptors/DescriptorPool.hpp"
#include "context/Context.hpp"


bool DescriptorPool::create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept
{
    const VkDescriptorPoolCreateInfo poolInfo =
        {.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
         .pNext         = nullptr,
         .flags         = 0,
         .maxSets       = 1,
         .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
         .pPoolSizes    = poolSizes.data()};

    return (vkCreateDescriptorPool(vkContext->get<VulkanDevice>()->getHandle(), &poolInfo, nullptr, &handle) == VK_SUCCESS);
}


bool DescriptorPool::allocateDescriptorSet(VkDescriptorSet* descriptorSet, const VkDescriptorSetLayout* layout) noexcept
{
    const VkDescriptorSetAllocateInfo allocateInfo = {.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                      .pNext          = nullptr,
                                                      .descriptorPool = handle,
                                                      .descriptorSetCount = 1,
                                                      .pSetLayouts        = layout};

    return (vkAllocateDescriptorSets(vkContext->get<VulkanDevice>()->getHandle(), &allocateInfo, descriptorSet) ==
            VK_SUCCESS);
}


void DescriptorPool::writeBufferInfo(const VkDescriptorBufferInfo* bufferInfo,
                                     VkDescriptorSet               descriptorSet,
                                     uint32_t                      dstBinding) noexcept
{
    const VkWriteDescriptorSet descriptorWrite =
        {.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
         .pNext            = nullptr,
         .dstSet           = descriptorSet,
         .dstBinding       = dstBinding,
         .dstArrayElement  = 0,
         .descriptorCount  = 1,
         .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
         .pImageInfo       = nullptr,
         .pBufferInfo      = bufferInfo,
         .pTexelBufferView = nullptr};

    vkUpdateDescriptorSets(vkContext->get<VulkanDevice>()->getHandle(), 1, &descriptorWrite, 0, nullptr);
}


void DescriptorPool::writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo,
                                               VkDescriptorSet              descriptorSet,
                                               uint32_t                     dstBinding) noexcept
{
    const VkWriteDescriptorSet descriptorWrite =
        {.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
         .pNext            = nullptr,
         .dstSet           = descriptorSet,
         .dstBinding       = dstBinding,
         .dstArrayElement  = 0,
         .descriptorCount  = 1,
         .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
         .pImageInfo       = imageInfo,
         .pBufferInfo      = nullptr,
         .pTexelBufferView = nullptr};

    vkUpdateDescriptorSets(vkContext->get<VulkanDevice>()->getHandle(), 1, &descriptorWrite, 0, nullptr);
}


void DescriptorPool::destroy() noexcept
{
    vkDestroyDescriptorPool(vkContext->get<VulkanDevice>()->getHandle(), handle, nullptr);
}