#include <array>
#include <vector>
#include <unordered_set>

#ifdef DEBUG
#include <cstdio>
#endif

#include "spdlog/spdlog.h"
#include <magic_enum/magic_enum.hpp>

#include "context/instance/VulkanInstance.hpp"

#ifdef DEBUG
    static std::array<const char*, 1> validation_layers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    static bool check_validation_layer_support() noexcept
    {
        VkResult result = VK_SUCCESS;

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, VK_NULL_HANDLE);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (uint32_t i = 0; i < validation_layers.size(); ++i)
        {
            const char* layerName = validation_layers[i];
            bool layerFound = false;

            for (uint32_t j = 0; j < layerCount; ++j)
            {
                if (strcmp(layerName, availableLayers[j].layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
                result = VK_ERROR_LAYER_NOT_PRESENT;
        }

        const auto info = magic_enum::enum_name(result);
        spdlog::info("VK_LAYER_KHRONOS_validation: {}", info);

        return (result == VK_SUCCESS);
    }
#endif // !DEBUG



VulkanInstance::VulkanInstance() noexcept:
    VulkanObject(VK_OBJECT_TYPE_INSTANCE)
{

}


VulkanInstance::~VulkanInstance() = default;


bool VulkanInstance::create() noexcept
{
    spdlog::info("Starting instance initialization");

#ifdef DEBUG
    if ( ! check_validation_layer_support() )
        return false;
#endif // !DEBUG

    std::vector<const char*> requiredExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#ifdef _WIN32
    requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef __linux__
    requiredExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

#ifdef DEBUG
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    for (const auto extensionName : requiredExtensions)
    {
        spdlog::info("Required vulkan API extension: {}", extensionName);
    }

    uint32_t availableExtensionCount;
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &availableExtensionCount, VK_NULL_HANDLE);

    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &availableExtensionCount, availableExtensions.data());

    std::unordered_set<std::string> deviceExtensions;
    
    for (const auto& it : availableExtensions)
    {
        deviceExtensions.insert(it.extensionName);
        spdlog::info("Available vulkan API extension: {}", it.extensionName);
    }

    for (const auto it : requiredExtensions)
    {
        if (deviceExtensions.find(it) == deviceExtensions.end())
        {
            spdlog::error("The vulkan API extension is not available: {}", it);

            return false;
        }	
    }

    const VkApplicationInfo appInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = VK_NULL_HANDLE,
        .pApplicationName   = "World3D",
        .applicationVersion = 1,
        .pEngineName        = "3D Engine",
        .engineVersion      = 1,
        .apiVersion         = VK_API_VERSION_1_3
    };

    {
        spdlog::info("Application name: {}", appInfo.pApplicationName);
        spdlog::info("Engine name: {}", appInfo.pEngineName);
        spdlog::info("Api version: {:.1f}", 1.3f);
    }

    VkInstanceCreateInfo instanceInfo = 
    {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = VK_NULL_HANDLE,
        .flags                   = 0,
        .pApplicationInfo        = &appInfo,
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = VK_NULL_HANDLE,
        .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

#ifdef DEBUG
    instanceInfo.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
    instanceInfo.ppEnabledLayerNames = validation_layers.data();

    const VkDebugUtilsMessengerCreateInfoEXT debugInfo = 
    {
    .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext           = VK_NULL_HANDLE,
    .flags           = 0,
    .messageSeverity = 0,
    .messageType     = 0,
    .pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
                            VkDebugUtilsMessageTypeFlagsEXT messageType, 
                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
                            void* pUserData)
                            {
                                printf("validation layer: %s\n", pCallbackData->pMessage);

                                return VK_FALSE;
                            },
    .pUserData = VK_NULL_HANDLE
    };

    instanceInfo.pNext = static_cast<const void*>(&debugInfo);
#endif // !DEBUG

    const auto result = vkCreateInstance(&instanceInfo, VK_NULL_HANDLE, &m_handle);
    spdlog::info("Completing instance initialization: {}", magic_enum::enum_name(result));

    return (result == VK_SUCCESS);
}


void VulkanInstance::destroy() noexcept
{
    if (m_handle)
        vkDestroyInstance(m_handle, VK_NULL_HANDLE);
}