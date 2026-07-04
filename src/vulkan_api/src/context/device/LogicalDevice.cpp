#include <vector>
#include <unordered_set>
#include <string>

#include "spdlog/spdlog.h"
#include <magic_enum/magic_enum.hpp>

#include "context/device/LogicalDevice.hpp"


LogicalDevice::LogicalDevice() noexcept:
    VulkanObject(VK_OBJECT_TYPE_DEVICE),
    m_queue(VK_NULL_HANDLE),
    m_queueFamilyIndex(0)
{

}


LogicalDevice::~LogicalDevice() = default;


bool LogicalDevice::create(VkPhysicalDevice physicalDevice) noexcept
{
    spdlog::info("Starting the creation of a logical device");

    VkPhysicalDeviceFeatures supportedFeatures;
    VkPhysicalDeviceFeatures enabledFeatures = {};

    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    enabledFeatures.samplerAnisotropy = supportedFeatures.samplerAnisotropy;
    enabledFeatures.fillModeNonSolid = supportedFeatures.fillModeNonSolid;

    {// Find main queue family index
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, VK_NULL_HANDLE);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        m_queueFamilyIndex = UINT32_MAX;

        for (uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            if (queueFamilies[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
            {
                spdlog::info("The main queue family index with flags is selected: {} | {}", 
                    magic_enum::enum_name(VK_QUEUE_GRAPHICS_BIT), magic_enum::enum_name(VK_QUEUE_TRANSFER_BIT));

                m_queueFamilyIndex = i;
                break;
            }
        }
    }

    if (m_queueFamilyIndex != UINT32_MAX)
    {
        const float queuePriority = 1.0f;

        const VkDeviceQueueCreateInfo queueInfo = 
        {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = VK_NULL_HANDLE,
            .flags            = 0,
            .queueFamilyIndex = m_queueFamilyIndex,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority
        };

        const std::array<const char*, 2> requiredExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        for (const char* ext : requiredExtensions)
            spdlog::info("Required device extension: {}", ext);
        
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, VK_NULL_HANDLE, &extensionCount, availableExtensions.data());

        std::unordered_set<std::string> deviceExtensions;

        for (const auto& it : availableExtensions)
        {
            deviceExtensions.insert(it.extensionName);
            spdlog::info("An extension for the device is available: {}", it.extensionName);
        }

        for (const auto& extension : requiredExtensions)
        {
            if(deviceExtensions.find(extension) == deviceExtensions.end())
            {
                spdlog::error("The device extension is not available: {}", extension);

                return false;
            }
        }

        const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature = 
        {
            .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
            .dynamicRendering = VK_TRUE
        };

        VkDeviceCreateInfo deviceInfo = 
        {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = &dynamicRenderingFeature,
            .flags                   = 0,
            .queueCreateInfoCount    = 1,
            .pQueueCreateInfos       = &queueInfo,
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = VK_NULL_HANDLE,
            .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data(),
            .pEnabledFeatures        = &enabledFeatures
        };
#ifdef DEBUG
        std::array<const char*, 1> validationLayers = { "VK_LAYER_KHRONOS_validation"};
        deviceInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        deviceInfo.ppEnabledLayerNames = validationLayers.data();
#endif

        const auto result = vkCreateDevice(physicalDevice, &deviceInfo, VK_NULL_HANDLE, &m_handle);

        if (result == VK_SUCCESS)
        {
            spdlog::info("Initialization of the device has been completed with the result: {}", magic_enum::enum_name(result));
            vkGetDeviceQueue(m_handle, m_queueFamilyIndex, 0, &m_queue);

            return true;
        }

        spdlog::error("Initialization of the device has been failed with the result: {}", magic_enum::enum_name(result));
        
        return false;
    }

    spdlog::error("A suitable graphics queue was not found.");

    return false;
}


void LogicalDevice::destroy() noexcept
{
    if (m_handle)
        vkDestroyDevice(m_handle, VK_NULL_HANDLE);
}


VkQueue LogicalDevice::getQueue() const noexcept
{
    return m_queue;
}


uint32_t LogicalDevice::getQueueFamilyIndex() const noexcept
{
    return m_queueFamilyIndex;
}