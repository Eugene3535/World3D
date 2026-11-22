#pragma once

#include "utils/VulkanHandle.hpp"


class VulkanPhysicalDevice:
    public VulkanHandle<VkPhysicalDevice>
{
public:
    VulkanPhysicalDevice() noexcept;
    ~VulkanPhysicalDevice();

    bool select(VkInstance instance) noexcept;

private:

};