#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vector>

#include <vulkan/vulkan.h>


class Swapchain
{
public:
    Swapchain() noexcept;

    bool create(VkSurfaceKHR surface) noexcept;
    void destroy() noexcept;

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

#endif // !SWAPCHAIN_HPP