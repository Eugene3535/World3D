#ifndef DESCRIPTOR_POOL_HPP
#define DESCRIPTOR_POOL_HPP

#include <vector>
#include <span>

#include "utils/Tools.hpp"

struct DescriptorPool
{
    bool create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept;
    bool allocateDescriptorSets(std::span<VkDescriptorSet> descriptorSets, const VkDescriptorSetLayout* layouts) noexcept;
    void writeBufferInfo(const VkDescriptorBufferInfo* bufferInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept;
    void writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept;
    void destroy() noexcept;

    VkDescriptorPool handle = VK_NULL_HANDLE;
    std::vector<VkDescriptorType> types;
};

#endif // !DESCRIPTOR_POOL_HPP