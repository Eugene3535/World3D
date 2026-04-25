#include <string>
#include <cstring>
#include <array>
#include <vector>
#include <unordered_set>

#ifdef DEBUG
#include <cstdio>
#endif

#include "context/Context.hpp"


#ifdef DEBUG
namespace
{
    std::array<const char*, 1> validation_layers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    VkResult check_validation_layer_support()
    {
        VkResult allLayersFound = VK_SUCCESS;

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, VK_NULL_HANDLE);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for(uint32_t i = 0; i < validation_layers.size(); ++i)
        {
            const char* layerName = validation_layers[i];
            bool layerFound = false;

            for(uint32_t j = 0; j < layerCount; ++j)
            {
                if (strcmp(layerName, availableLayers[j].layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if(!layerFound)
                allLayersFound = VK_ERROR_LAYER_NOT_PRESENT;
        }

        return allLayersFound;
    }
}

#endif // !DEBUG



bool VulkanContext::createInstance() noexcept
{
#ifdef DEBUG
    if (check_validation_layer_support() != VK_SUCCESS)
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

    uint32_t availableExtensionCount;
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &availableExtensionCount, VK_NULL_HANDLE);

    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &availableExtensionCount, availableExtensions.data());

    std::unordered_set<std::string> deviceExtensions;

    for (const auto& it : availableExtensions)
        deviceExtensions.insert(it.extensionName);

    for (const auto it : requiredExtensions)
        if (deviceExtensions.find(it) == deviceExtensions.end())	
            return false;

    const VkApplicationInfo appInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = VK_NULL_HANDLE,
        .pApplicationName   = "World3D",
        .applicationVersion = 1,
        .pEngineName        = "Shining Engine",
        .engineVersion      = 1,
        .apiVersion         = VK_API_VERSION_1_3
    };

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

    instanceInfo.pNext = (const void*)(&debugInfo);
#endif // !DEBUG

    return (vkCreateInstance(&instanceInfo, VK_NULL_HANDLE, &instance) == VK_SUCCESS);
}


bool VulkanContext::selectVideoCard() noexcept
{    
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, VK_NULL_HANDLE);

    if (deviceCount)
    {
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for(uint32_t i = 0; i < deviceCount; ++i)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(devices[i], &properties);

            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                GPU = devices[i];

            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                GPU = devices[i];
                
                return true;
            }
        }
    }

    return GPU ? true : false;
}


bool VulkanContext::createDevice() noexcept
{
    VkPhysicalDeviceFeatures supportedFeatures;
    VkPhysicalDeviceFeatures enabledFeatures = {};

    vkGetPhysicalDeviceFeatures(GPU, &supportedFeatures);

    if (supportedFeatures.samplerAnisotropy)
        enabledFeatures.samplerAnisotropy = VK_TRUE;

    if (supportedFeatures.fillModeNonSolid)
        enabledFeatures.fillModeNonSolid = VK_TRUE;

    {// Find main queue family index
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, VK_NULL_HANDLE);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, queueFamilies.data());

        mainQueueFamilyIndex = UINT32_MAX;

        for (uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            if (queueFamilies[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
            {
                mainQueueFamilyIndex = i;
                break;
            }
        }
    }

	if(mainQueueFamilyIndex != UINT32_MAX)
    {
        const float queuePriority = 1.0f;

    	const VkDeviceQueueCreateInfo queueInfo = 
        {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = VK_NULL_HANDLE,
            .flags            = 0,
            .queueFamilyIndex = mainQueueFamilyIndex,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority
        };

        const std::array<const char*, 2> requiredExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(GPU, VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(GPU, VK_NULL_HANDLE, &extensionCount, availableExtensions.data());

        std::unordered_set<std::string> deviceExtensions;

        for (const auto& it : availableExtensions)
            deviceExtensions.insert(it.extensionName);

        for (const auto& extension : requiredExtensions)
            if(deviceExtensions.find(extension) == deviceExtensions.end())
                return false;

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
    	deviceInfo.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
    	deviceInfo.ppEnabledLayerNames = validation_layers.data();
#endif

        if(vkCreateDevice(GPU, &deviceInfo, VK_NULL_HANDLE, &device) == VK_SUCCESS)
        {
            vkGetDeviceQueue(device, mainQueueFamilyIndex, 0, &queue);

            return true;
        }
    }

    return false;
}


void VulkanContext::destroy() noexcept
{
    if(device)
        vkDestroyDevice(device, VK_NULL_HANDLE);

    if(instance)
        vkDestroyInstance(instance, VK_NULL_HANDLE);
}