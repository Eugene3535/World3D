#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <vector>

#include <GLFW/glfw3.h>

#include "vulkan_api/context/Context.hpp"


struct VK_API MainView
{
    bool create(GLFWwindow* window) noexcept;
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