#include <vector>

#include "spdlog/spdlog.h"
#include <magic_enum/magic_enum.hpp>

#include "context/gpu/PhysicalDevice.hpp"


PhysicalDevice::PhysicalDevice() noexcept:
    VulkanObject(VK_OBJECT_TYPE_PHYSICAL_DEVICE)
{

}


PhysicalDevice::~PhysicalDevice() = default;


bool PhysicalDevice::create(VkInstance instance) noexcept
{
    spdlog::info("Start selecting a physical device");

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, VK_NULL_HANDLE);

    if (deviceCount)
    {
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        spdlog::info("Physical devices found: {}", deviceCount);
        uint32_t i = 0;

        for(; i < deviceCount; ++i)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(devices[i], &properties);

            spdlog::info("Physical device available: {}, type: {}", properties.deviceName, magic_enum::enum_name(properties.deviceType));

            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                m_handle = devices[i];

            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                m_handle = devices[i];

                break;
            }
        }

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        spdlog::info("The physical device is selected: {}, type: {}", properties.deviceName, magic_enum::enum_name(properties.deviceType));
    }

    return (m_handle != VK_NULL_HANDLE);
}