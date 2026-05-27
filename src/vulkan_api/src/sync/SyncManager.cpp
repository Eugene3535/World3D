#include "context/Context.hpp"
#include "sync/SyncManager.hpp"


bool SyncManager::create() noexcept
{
    auto logicalDevice = vkContext->getLogicalDevice();

    const VkSemaphoreCreateInfo semaphoreInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0
    };

    const VkFenceCreateInfo fenceInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if(vkCreateSemaphore(logicalDevice, &semaphoreInfo, VK_NULL_HANDLE, &imageAvailableSemaphores[i]) != VK_SUCCESS)
            return false;

        if(vkCreateSemaphore(logicalDevice, &semaphoreInfo, VK_NULL_HANDLE, &renderFinishedSemaphores[i]) != VK_SUCCESS)
            return false;

        if(vkCreateFence(logicalDevice, &fenceInfo, VK_NULL_HANDLE, &inFlightFences[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}


void SyncManager::destroy() noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], VK_NULL_HANDLE);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], VK_NULL_HANDLE);
        vkDestroyFence(logicalDevice, inFlightFences[i], VK_NULL_HANDLE);
    }
}