#pragma once

#include "context/instance/VulkanInstance.hpp"
#include "context/gpu/PhysicalDevice.hpp"
#include "context/device/LogicalDevice.hpp"

class VulkanContext final
{
public:
    VulkanContext() noexcept;

    bool create() noexcept;
    void destroy() noexcept;

    static VulkanContext* getContext() noexcept;

    const VulkanInstance* getInstance()       const noexcept;
    const PhysicalDevice* getPhysicalDevice() const noexcept;
    const LogicalDevice*  getLogicalDevice()  const noexcept;

private:
    VulkanInstance m_instance;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
};

#define vkContext VulkanContext::getContext()