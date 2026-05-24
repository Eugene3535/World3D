#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <vector>

#include "context/Context.hpp"


class MainView
{
public:
    MainView(const class VulkanContext& ctx) noexcept;

    bool createSurface(uint64_t windowHandle) noexcept;
    bool recreate() noexcept;
    void destroy() noexcept;

    const VulkanContext& context;

    VkSurfaceKHR   surface   = nullptr;
    VkSwapchainKHR swapchain = nullptr;

    std::vector<VkImage>     images;
    std::vector<VkImageView> imageViews;

    struct
    {
        VkImage        image       = nullptr;
        VkDeviceMemory imageMemory = nullptr;
        VkImageView    imageView   = nullptr;
    } depthBuffer;

    VkFormat   format;
    VkExtent2D extent;
};

#endif // !MAIN_VIEW_HPP