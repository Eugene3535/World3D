#include "context/Context.hpp"
#include "command_pool/CommandBufferPool.hpp"


bool CommandBufferPool::create() noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();

    const VkCommandPoolCreateInfo poolInfo = 
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = VK_NULL_HANDLE,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vkContext->getQueueFamilyIndex()
    };

    if (vkCreateCommandPool(logicalDevice, &poolInfo, VK_NULL_HANDLE, &handle) != VK_SUCCESS)
        return false;

    const VkCommandBufferAllocateInfo allocInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = VK_NULL_HANDLE,
        .commandPool        = handle,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32_t>(commandBuffers.size())
    };

    return (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()) == VK_SUCCESS);
}


void CommandBufferPool::destroy() noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();
    vkDestroyCommandPool(logicalDevice, handle, VK_NULL_HANDLE);
}