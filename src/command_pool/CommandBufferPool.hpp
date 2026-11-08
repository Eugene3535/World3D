#pragma once

#include <array>

#include "utils/Tools.hpp"


struct CommandBufferPool
{
    bool create() noexcept;
    void destroy() noexcept;

    VkCommandPool                                     handle         = nullptr;
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> commandBuffers = {nullptr, nullptr};
};