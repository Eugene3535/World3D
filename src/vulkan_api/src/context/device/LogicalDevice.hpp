#pragma once

#include "context/VulkanObject.hpp"


class LogicalDevice:
    public VulkanObject<VkDevice>
{
public:
    LogicalDevice() noexcept;
    ~LogicalDevice();

    bool create(VkPhysicalDevice physicalDevice) noexcept;
    void destroy() noexcept;

    VkQueue  getQueue()            const noexcept;
    uint32_t getQueueFamilyIndex() const noexcept;

private:
    VkQueue  m_queue;
    uint32_t m_queueFamilyIndex;
};