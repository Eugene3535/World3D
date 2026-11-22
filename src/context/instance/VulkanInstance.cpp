#include <array>
#include <unordered_set>
#include <vector>

#include "spdlog/spdlog.h"
#include <magic_enum/magic_enum.hpp>

#include "context/instance/VulkanInstance.hpp"


#ifdef DEBUG

static bool check_validation_layer_support(const std::array<const char*, 1>& validationLayers) noexcept
{
    VkResult result = VK_SUCCESS;

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (uint32_t i = 0; i < validationLayers.size(); ++i)
    {
        const char* layerName  = validationLayers[i];
        bool        layerFound = false;

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


VulkanInstance::VulkanInstance() noexcept = default;
VulkanInstance::~VulkanInstance()         = default;


bool VulkanInstance::create() noexcept
{
    spdlog::info("Starting instance initialization");

#ifdef DEBUG
    if (!check_validation_layer_support(validationLayers))
        return false;
#endif // !DEBUG

    std::vector<const char*> requiredExtensions = {VK_KHR_SURFACE_EXTENSION_NAME};

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
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

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

    const VkApplicationInfo appInfo = {.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                       .pNext              = nullptr,
                                       .pApplicationName   = "World3D",
                                       .applicationVersion = 1,
                                       .pEngineName        = "Engine_3D",
                                       .engineVersion      = 1,
                                       .apiVersion         = VK_API_VERSION_1_3};

    {
        spdlog::info("Application name: {}", appInfo.pApplicationName);
        spdlog::info("Engine name: {}", appInfo.pEngineName);
        spdlog::info("Api version: {:.1f}", 1.3f);
    }

    VkInstanceCreateInfo instanceInfo =
        {.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
         .pNext                   = nullptr,
         .flags                   = 0,
         .pApplicationInfo        = &appInfo,
         .enabledLayerCount       = 0,
         .ppEnabledLayerNames     = nullptr,
         .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
         .ppEnabledExtensionNames = requiredExtensions.data()};

#ifdef DEBUG
    instanceInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
    instanceInfo.ppEnabledLayerNames = validationLayers.data();

    const VkDebugUtilsMessengerCreateInfoEXT debugInfo =
        {.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
         .pNext           = nullptr,
         .flags           = 0,
         .messageSeverity = 0,
         .messageType     = 0,
         .pfnUserCallback =
             [](VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT             messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void*                                       pUserData)
         {
             printf("validation layer: %s\n", pCallbackData->pMessage);

             return VK_FALSE;
         },
         .pUserData = nullptr};

    instanceInfo.pNext = static_cast<const void*>(&debugInfo);
#endif // !DEBUG

    const auto result = vkCreateInstance(&instanceInfo, nullptr, &m_handle);
    spdlog::info("Completing instance initialization: {}", magic_enum::enum_name(result));

    return (result == VK_SUCCESS);
}