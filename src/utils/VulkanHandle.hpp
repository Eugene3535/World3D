#pragma once

#include <array>

#include <vulkan/vulkan.h>


template<class T>
class VulkanHandle
{
public:
#ifdef DEBUG
    static constexpr std::array<const char*, 1> validationLayers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };
#endif

    VulkanHandle() noexcept = default;
    virtual ~VulkanHandle() = default;

    T getHandle() const noexcept
    {
        return m_handle;
    }

protected:
    T m_handle{VK_NULL_HANDLE};
};