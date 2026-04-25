#include <memory>
#include <cstring>

#include <cglm/util.h>

#include "utils/Tools.hpp"
#include "presentation/MainView.hpp"


struct SwapChainSupportDetails
{
    VkPresentModeKHR getPresentMode() noexcept
    {
        for (const auto mode : presentModes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
    
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkSurfaceFormatKHR getSurfaceFormat() noexcept
    {
        if(surfaceFormats.empty())
            return {};

        for (const auto& surfaceFmt : surfaceFormats)
            if (surfaceFmt.format == VK_FORMAT_B8G8R8A8_SRGB &&
                surfaceFmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    return surfaceFmt;
        
        return surfaceFormats[0];
    }

    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR>   presentModes;
};


namespace
{
    std::unique_ptr<SwapChainSupportDetails> query_swapchain_support(MainView* view) noexcept
    {
        VkPhysicalDevice gpu = view->context->GPU;
        VkSurfaceKHR surface = view->surface;

        auto details = std::make_unique<SwapChainSupportDetails>();
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &details->capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, VK_NULL_HANDLE);

        details->surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, details->surfaceFormats.data());
        
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, VK_NULL_HANDLE);

        details->presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, details->presentModes.data());
        
        return details;
    }


    VkExtent2D choose_swap_extent(const SwapChainSupportDetails* details, const VkExtent2D* currentExtent)
    {
        VkExtent2D actualExtent = { 0, 0 };

        if (details->capabilities.currentExtent.width != UINT_MAX)
        {
            return details->capabilities.currentExtent;
        }
        else
        {
            memcpy(&actualExtent, currentExtent, sizeof(VkExtent2D));
            actualExtent.width  = glm_clamp(actualExtent.width, details->capabilities.minImageExtent.width, details->capabilities.maxImageExtent.width);
            actualExtent.height = glm_clamp(actualExtent.height, details->capabilities.minImageExtent.height, details->capabilities.maxImageExtent.height);
        }

        return actualExtent;
    }


    bool create_depth_resources(MainView* view)
    {
        bool result = false;
        VkDevice device = view->context->device;
        VkFormat depthFormat = vktools::find_depth_format(view->context->GPU);

        if(depthFormat != VK_FORMAT_UNDEFINED)
        {
            result = vktools::create_image_2D(view->extent, 
                                              depthFormat, 
                                              VK_IMAGE_TILING_OPTIMAL, 
                                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                              &view->depth.image, 
                                              &view->depth.imageMemory, 
                                              view->context->GPU, 
                                              device);

            if (result)
                result = vktools::create_image_view_2D(device, view->depth.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &view->depth.imageView);
        }

        return result;
    }
}



bool MainView::createSurface(uint64_t windowHandle) noexcept
{
    if(surface)
        return true;

#ifdef _WIN32
    const VkWin32SurfaceCreateInfoKHR surfaceInfo = 
    {
        .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext     = VK_NULL_HANDLE,
        .flags     = 0,
        .hinstance = GetModuleHandle(VK_NULL_HANDLE),
        .hwnd      = reinterpret_cast<HWND>(windowHandle)
    };

    return (vkCreateWin32SurfaceKHR(context->instance, &surfaceInfo, VK_NULL_HANDLE, &surface) == VK_SUCCESS);
#endif

#ifdef __linux__
	xcb_connection_t* connection = xcb_connect(VK_NULL_HANDLE, VK_NULL_HANDLE);

    if (xcb_connection_has_error(connection))
        return false;
        
    const VkXcbSurfaceCreateInfoKHR surfaceInfo =
    {
        .sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .pNext      = VK_NULL_HANDLE,
        .flags      = 0,
        .connection = connection,
        .window     = static_cast<xcb_window_t>(windowHandle)
    };

    return (vkCreateXcbSurfaceKHR(context->instance, &surfaceInfo, VK_NULL_HANDLE, &surface) == VK_SUCCESS);
#endif

    return false;
}


bool MainView::recreate(bool useDepth) noexcept
{
    if (surface)
    {
        VkDevice device = context->device;

        if (swapchain)
        {
            for(const auto imageView : imageViews)
                vkDestroyImageView(device, imageView, VK_NULL_HANDLE);

            vkDestroySwapchainKHR(device, swapchain, VK_NULL_HANDLE);
            swapchain = VK_NULL_HANDLE;
        }

        auto swapChainSupport = query_swapchain_support(this);
        const uint32_t minImageCount = swapChainSupport->capabilities.minImageCount;

        format = swapChainSupport->getSurfaceFormat().format;
        extent = choose_swap_extent(swapChainSupport.get(), &extent);

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
            .oldSwapchain          = swapchain
        };

        if (vkCreateSwapchainKHR(device, &swapchainInfo, VK_NULL_HANDLE, &swapchain) == VK_SUCCESS)
        {
            uint32_t imageCount = 0;
            
            if(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, VK_NULL_HANDLE) != VK_SUCCESS)
                return false;

            if(images.empty())
                images.resize(imageCount);
            
            if(imageViews.empty())
                imageViews.resize(imageCount);
            
            if (vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()) == VK_SUCCESS)
            {
                for (uint32_t i = 0; i < imageViews.size(); ++i)
                {
                    if (!vktools::create_image_view_2D(device, images[i], format, VK_IMAGE_ASPECT_COLOR_BIT, &imageViews[i]))
                        return false;  
                }

                if (useDepth)
                {
                    if (depth.imageView)
                        vkDestroyImageView(device, depth.imageView, VK_NULL_HANDLE);

                    if (depth.image)
                        vkDestroyImage(device, depth.image, VK_NULL_HANDLE);

                    if (depth.imageMemory)
                        vkFreeMemory(device, depth.imageMemory, VK_NULL_HANDLE);

                    if(!create_depth_resources(this))
                        return false;
                }
            }
        }
    }

    return true;
}


void MainView::destroy() noexcept
{
    VkDevice device = context->device;

    if(swapchain)
    {
        for(const auto imageView : imageViews)
            vkDestroyImageView(device, imageView, VK_NULL_HANDLE);

        vkDestroySwapchainKHR(device, swapchain, VK_NULL_HANDLE);
    }

    if (depth.imageView)
        vkDestroyImageView(device, depth.imageView, VK_NULL_HANDLE);

    if (depth.image)
        vkDestroyImage(device, depth.image, VK_NULL_HANDLE);

    if (depth.imageMemory)
        vkFreeMemory(device, depth.imageMemory, VK_NULL_HANDLE);

    if(surface)
        vkDestroySurfaceKHR(context->instance, surface, VK_NULL_HANDLE);
}