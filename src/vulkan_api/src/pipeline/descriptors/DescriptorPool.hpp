#ifndef DESCRIPTOR_POOL_HPP
#define DESCRIPTOR_POOL_HPP


#include <span>

#include "utils/Tools.hpp"

class DescriptorPool
{
public:
    bool create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept;
    void destroy() noexcept;

    bool allocateDescriptorSets(std::span<VkDescriptorSet> descriptorSets, const VkDescriptorSetLayout* layouts) noexcept;
    void writeBufferInfo(const VkDescriptorBufferInfo* bufferInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept;
    void writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept;

private:
    VkDescriptorPool m_handle = VK_NULL_HANDLE;
};

#endif // !DESCRIPTOR_POOL_HPP