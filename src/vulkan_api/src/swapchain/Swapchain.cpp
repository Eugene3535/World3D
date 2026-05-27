#include <memory>
#include <cstring>

#include "utils/Tools.hpp"
#include "context/Context.hpp"
#include "swapchain/Swapchain.hpp"


Swapchain::Swapchain() noexcept:
    handle(VK_NULL_HANDLE)
{

}


bool Swapchain::create(VkSurfaceKHR surface) noexcept
{
    auto context = vkContext;
    VkDevice device = context->getLogicalDevice();

    VkSwapchainKHR oldSwapchain = handle;

    if (handle)
    {
        for(const auto imageView : imageViews)
            vkDestroyImageView(device, imageView, VK_NULL_HANDLE);

        handle = VK_NULL_HANDLE;
    }

    auto swapChainSupport = vktools::SwapChainSupportDetails::querySupport(context->getPhysicalDevice(), surface);
    const uint32_t minImageCount = swapChainSupport->capabilities.minImageCount;

    format = swapChainSupport->getSurfaceFormat().format;
    extent = vktools::SwapChainSupportDetails::chooseSwapExtent(swapChainSupport.get(), &extent);

    const VkSwapchainCreateInfoKHR swapchainInfo = 
    {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = VK_NULL_HANDLE,
        .flags                 = 0,
        .surface               = surface,
        .minImageCount         = minImageCount,
        .imageFormat           = format,
        .imageColorSpace       = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent           = extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = VK_NULL_HANDLE,
        .preTransform          = swapChainSupport->capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = swapChainSupport->getPresentMode(),
        .clipped               = VK_TRUE,
        .oldSwapchain          = oldSwapchain
    };

    if (vkCreateSwapchainKHR(device, &swapchainInfo, VK_NULL_HANDLE, &handle) == VK_SUCCESS)
    {
        if (oldSwapchain)
            vkDestroySwapchainKHR(device, oldSwapchain, VK_NULL_HANDLE);

        uint32_t imageCount = 0;
        
        if (vkGetSwapchainImagesKHR(device, handle, &imageCount, VK_NULL_HANDLE) != VK_SUCCESS)
            return false;

        if (images.empty())
            images.resize(imageCount);
        
        if (imageViews.empty())
            imageViews.resize(imageCount);
        
        if (vkGetSwapchainImagesKHR(device, handle, &imageCount, images.data()) == VK_SUCCESS)
        {
            for (uint32_t i = 0; i < imageViews.size(); ++i)
            {
                if (!vktools::create_image_view_2D(device, images[i], format, VK_IMAGE_ASPECT_COLOR_BIT, &imageViews[i]))
                    return false;  
            }
//  Depth buffering
            if (depthBuffer.imageView)
                vkDestroyImageView(device, depthBuffer.imageView, VK_NULL_HANDLE);

            if (depthBuffer.image)
                vkDestroyImage(device, depthBuffer.image, VK_NULL_HANDLE);

            if (depthBuffer.imageMemory)
                vkFreeMemory(device, depthBuffer.imageMemory, VK_NULL_HANDLE);

            if (const VkFormat depthFormat = vktools::find_depth_format(context->getPhysicalDevice()); depthFormat != VK_FORMAT_UNDEFINED)
            {
                bool result = vktools::create_image_2D(extent, 
                                                        depthFormat, 
                                                        VK_IMAGE_TILING_OPTIMAL, 
                                                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                                        &depthBuffer.image, 
                                                        &depthBuffer.imageMemory, 
                                                        context->getPhysicalDevice(), 
                                                        device);

                if (result)
                    result = vktools::create_image_view_2D(device, depthBuffer.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &depthBuffer.imageView);
            }

            return true;
        }
    }

    return false;
}


void Swapchain::destroy() noexcept
{
    VkDevice device = vkContext->getLogicalDevice();

    if(handle)
    {
        for(const auto imageView : imageViews)
            vkDestroyImageView(device, imageView, VK_NULL_HANDLE);

        vkDestroySwapchainKHR(device, handle, VK_NULL_HANDLE);
    }

    if (depthBuffer.imageView)
        vkDestroyImageView(device, depthBuffer.imageView, VK_NULL_HANDLE);

    if (depthBuffer.image)
        vkDestroyImage(device, depthBuffer.image, VK_NULL_HANDLE);

    if (depthBuffer.imageMemory)
        vkFreeMemory(device, depthBuffer.imageMemory, VK_NULL_HANDLE);
}