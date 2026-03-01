#include "command_pool/CommandBufferPool.hpp"


bool CommandBufferPool::create(VkDevice device, uint32_t queueFamilyIndex) noexcept
{
    const VkCommandPoolCreateInfo poolInfo = 
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = VK_NULL_HANDLE,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndex
    };

    if (vkCreateCommandPool(device, &poolInfo, VK_NULL_HANDLE, &handle) != VK_SUCCESS)
        return false;

    const VkCommandBufferAllocateInfo allocInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = VK_NULL_HANDLE,
        .commandPool        = handle,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32_t>(commandBuffers.size())
    };

    return (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) == VK_SUCCESS);
}


void CommandBufferPool::destroy(VkDevice device) noexcept
{
    vkDestroyCommandPool(device, handle, VK_NULL_HANDLE);
}