#include "command_pool/CommandBufferPool.hpp"
#include "context/Context.hpp"


bool CommandBufferPool::create() noexcept
{
    const auto* logicalDevice = vkContext->get<VulkanDevice>();
    const auto device = logicalDevice->getHandle();

    const VkCommandPoolCreateInfo poolInfo = {.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                              .pNext            = nullptr,
                                              .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                              .queueFamilyIndex = logicalDevice->getQueueFamilyIndex()};

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &handle) != VK_SUCCESS)
        return false;

    const VkCommandBufferAllocateInfo allocInfo = {.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                   .pNext              = nullptr,
                                                   .commandPool        = handle,
                                                   .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                   .commandBufferCount = static_cast<uint32_t>(commandBuffers.size())};

    return (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) == VK_SUCCESS);
}


void CommandBufferPool::destroy() noexcept
{
    vkDestroyCommandPool(vkContext->get<VulkanDevice>()->getHandle(), handle, nullptr);
}