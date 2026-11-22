#pragma once

#include "utils/VulkanHandle.hpp"


class VulkanDevice:
    public VulkanHandle<VkDevice>
{
public:
    VulkanDevice() noexcept;
    ~VulkanDevice();

    bool create(VkPhysicalDevice gpu) noexcept;

    VkQueue getGraphicsQueue() const noexcept;
    uint32_t getQueueFamilyIndex() const noexcept;

private:
    VkQueue  m_queue{VK_NULL_HANDLE};
    uint32_t m_queueFamilyIndex{0};
};