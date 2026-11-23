#pragma once

#include <span>

#include "utils/Tools.hpp"

struct DescriptorPool
{
    bool create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept;
    bool allocateDescriptorSet(VkDescriptorSet* descriptorSet, const VkDescriptorSetLayout* layout) noexcept;
    void writeBufferInfo(const VkDescriptorBufferInfo* bufferInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept;
    void writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo,
                                   VkDescriptorSet              descriptorSet,
                                   uint32_t                     dstBinding) noexcept;
    void destroy() noexcept;

    VkDescriptorPool handle = nullptr;
};