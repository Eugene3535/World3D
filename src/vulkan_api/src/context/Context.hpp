#pragma once

#include <type_traits>

#include <vulkan/vulkan.h>

class VulkanContext final
{
public:
    VulkanContext() noexcept;

    bool create() noexcept;
    void destroy() noexcept;

    template<class T> 
    T get() const noexcept
    {
        if constexpr (std::is_same_v<T, VkInstance>)
		    return m_instance;

        if constexpr (std::is_same_v<T, VkPhysicalDevice>)
		    return m_physicalDevice;

        if constexpr (std::is_same_v<T, VkDevice>)
		    return m_logicalDevice;

        if constexpr (std::is_same_v<T, VkQueue>)
		    return m_queue;

        return {};
    }

    uint32_t getQueueFamilyIndex() const noexcept;

    static VulkanContext* getContext() noexcept;

private:
    bool createInstance()  noexcept;
    bool selectVideoCard() noexcept;
    bool createDevice()    noexcept;

    VkInstance       m_instance;
    VkPhysicalDevice m_physicalDevice;
    VkDevice         m_logicalDevice;
    VkQueue          m_queue;
    uint32_t         m_queueFamilyIndex;
};

#define vkContext VulkanContext::getContext()