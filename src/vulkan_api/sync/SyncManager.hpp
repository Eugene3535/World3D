#ifndef SYNC_MANAGER_HPP
#define SYNC_MANAGER_HPP

#include <array>

#include "vulkan_api/Export.hpp"
#include "vulkan_api/utils/Tools.hpp"


struct VK_API SyncManager
{
    bool create(VkDevice device) noexcept;
    void destroy(VkDevice device) noexcept;

    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores = { VK_NULL_HANDLE, VK_NULL_HANDLE };;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores = { VK_NULL_HANDLE, VK_NULL_HANDLE };;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>     inFlightFences           = { VK_NULL_HANDLE, VK_NULL_HANDLE };;
    uint32_t    currentFrame = 0;
};

#endif // !SYNC_MANAGER_HPP