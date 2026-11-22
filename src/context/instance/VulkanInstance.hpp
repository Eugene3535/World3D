#pragma once

#include "utils/VulkanHandle.hpp"


class VulkanInstance : public VulkanHandle<VkInstance>
{
public:
    VulkanInstance() noexcept;
    ~VulkanInstance();

    bool create() noexcept;

private:
};