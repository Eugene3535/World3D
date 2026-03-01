#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <vector>

#include "context/Context.hpp"


struct MainView
{
    bool create(uint64_t windowHandle) noexcept;
    bool recreate(bool useDepth) noexcept;
    void destroy() noexcept;

    const VulkanContext* context = VK_NULL_HANDLE;

    VkSurfaceKHR   surface   = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    std::vector<VkImage>     images;
    std::vector<VkImageView> imageViews;

    struct
    {
        VkImage        image       = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        VkImageView    imageView   = VK_NULL_HANDLE;
    } depth;

    VkFormat   format;
    VkExtent2D extent;
};

#endif // !MAIN_VIEW_HPP