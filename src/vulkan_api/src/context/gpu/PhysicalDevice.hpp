#pragma once

#include "context/VulkanObject.hpp"


class PhysicalDevice:
    public VulkanObject<VkPhysicalDevice>
{
public:
    PhysicalDevice() noexcept;
    ~PhysicalDevice();

    bool create(VkInstance instance) noexcept;
};