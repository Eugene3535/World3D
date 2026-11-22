#include <array>
#include <cassert>
#include <cstring>
#include <string>
#include <unordered_set>
#include <vector>

#include "spdlog/spdlog.h"
#include <magic_enum/magic_enum.hpp>

#include "context/Context.hpp"
#include "utils/Tools.hpp"

static VulkanContext* g_vulkanContext;


VulkanContext::VulkanContext() noexcept
{
    assert(g_vulkanContext == nullptr);
    g_vulkanContext = this;
}


bool VulkanContext::create() noexcept
{
    if (!m_instance.create())
        return false;

    if (!m_physicalDevice.select(m_instance.getHandle()))
        return false;

    if (!m_logicalDevice.create(m_physicalDevice.getHandle()))
        return false;

    return true;
}


void VulkanContext::destroy() noexcept
{
    if (auto device = m_logicalDevice.getHandle())
        vkDestroyDevice(device, nullptr);

    if (auto instance = m_instance.getHandle())
        vkDestroyInstance(instance, nullptr);
}


VulkanContext* VulkanContext::getContext() noexcept
{
    return g_vulkanContext;
}