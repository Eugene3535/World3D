#pragma once

#include "context/VulkanObject.hpp"


class VulkanInstance:
    public VulkanObject<VkInstance>
{
public:
    VulkanInstance() noexcept;
    ~VulkanInstance();

    bool create() noexcept;
    void destroy() noexcept;
};