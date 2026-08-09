#pragma once

#include <array>

#include "utils/Tools.hpp"


struct SyncManager
{
    bool create() noexcept;
    void destroy() noexcept;

    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores = { VK_NULL_HANDLE, VK_NULL_HANDLE };
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores = { VK_NULL_HANDLE, VK_NULL_HANDLE };
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>     inFlightFences           = { VK_NULL_HANDLE, VK_NULL_HANDLE };
    uint32_t currentFrame = 0;
};