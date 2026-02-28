#ifndef DESCRIPTOR_POOL_HPP
#define DESCRIPTOR_POOL_HPP

#include <vector>
#include <span>

#include "utils/Tools.hpp"

struct DescriptorPool
{
    bool create(std::span<const VkDescriptorPoolSize> poolSizes, VkDevice device) noexcept;
    bool allocateDescriptorSets(std::span<VkDescriptorSet> descriptorSets, const VkDescriptorSetLayout* layouts, VkDevice device) noexcept;
    void writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding, VkDevice device) noexcept;
    void destroy(VkDevice device) noexcept;

    VkDescriptorPool handle = VK_NULL_HANDLE;
    std::vector<VkDescriptorType> types;
};

#endif // !DESCRIPTOR_POOL_HPP