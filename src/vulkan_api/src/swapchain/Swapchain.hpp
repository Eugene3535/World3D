#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <vector>

#include "context/Context.hpp"


class Swapchain
{
public:
    Swapchain(const VulkanContext& ctx) noexcept;

    bool create(VkSurfaceKHR surface) noexcept;
    void destroy() noexcept;

    const VulkanContext& context;

    VkSwapchainKHR           handle;
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