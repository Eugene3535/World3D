#pragma once

#include <type_traits>

#include "context/device/VulkanDevice.hpp"
#include "context/gpu/VulkanPhysicalDevice.hpp"
#include "context/instance/VulkanInstance.hpp"


class VulkanContext final
{
public:
    VulkanContext() noexcept;

    bool create() noexcept;
    void destroy() noexcept;

    template <class T>
    const T* get() const noexcept
    {
        if constexpr (std::is_same_v<T, VulkanInstance>)
            return &m_instance;

        if constexpr (std::is_same_v<T, VulkanPhysicalDevice>)
            return &m_physicalDevice;

        if constexpr (std::is_same_v<T, VulkanDevice>)
            return &m_logicalDevice;

        return nullptr;
    }

    static VulkanContext* getContext() noexcept;

private:
    VulkanInstance       m_instance;
    VulkanPhysicalDevice m_physicalDevice;
    VulkanDevice         m_logicalDevice;
};

#define vkContext VulkanContext::getContext()