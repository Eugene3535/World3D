#pragma once

#include <array>

#include "utils/Tools.hpp"


struct SyncManager
{
    bool create() noexcept;
    void destroy() noexcept;

    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores = {nullptr, nullptr};
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores = {nullptr, nullptr};
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>     inFlightFences           = {nullptr, nullptr};
    uint32_t                                      currentFrame             = 0;
};