#ifndef SYNC_MANAGER_HPP
#define SYNC_MANAGER_HPP

#include <array>

#include "utils/Tools.hpp"


struct SyncManager
{
    bool create(VkDevice device) noexcept;
    void destroy(VkDevice device) noexcept;

    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores = { VK_NULL_HANDLE, VK_NULL_HANDLE };;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores = { VK_NULL_HANDLE, VK_NULL_HANDLE };;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>     inFlightFences           = { VK_NULL_HANDLE, VK_NULL_HANDLE };;
    uint32_t    currentFrame = 0;
};

#endif // !SYNC_MANAGER_HPP