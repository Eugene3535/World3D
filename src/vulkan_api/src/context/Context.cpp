#include <string>
#include <cstring>
#include <cassert>
#include <array>
#include <vector>
#include <unordered_set>

#include "spdlog/spdlog.h"
#include <magic_enum/magic_enum.hpp>

#include "utils/Tools.hpp"
#include "context/Context.hpp"

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

    if (!m_physicalDevice.create(m_instance.getHandle()))
        return false;

    if (!m_logicalDevice.create(m_physicalDevice.getHandle()))
        return false;

    return true;
}


void VulkanContext::destroy() noexcept
{
    m_logicalDevice.destroy();
    m_instance.destroy();
}


VulkanContext* VulkanContext::getContext() noexcept
{
    return g_vulkanContext;
}


const VulkanInstance* VulkanContext::getInstance() const noexcept
{
    return &m_instance;
}

const PhysicalDevice* VulkanContext::getPhysicalDevice() const noexcept
{
    return &m_physicalDevice;
}

const LogicalDevice* VulkanContext::getLogicalDevice() const noexcept
{
    return &m_logicalDevice;
}