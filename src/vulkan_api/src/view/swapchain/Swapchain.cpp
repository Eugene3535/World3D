#include <memory>
#include <array>

#include <cglm/util.h>

#include "utils/Tools.hpp"
#include "context/Context.hpp"
#include "view/swapchain/Swapchain.hpp"


struct SwapChainSupportDetails
{
    static std::shared_ptr<SwapChainSupportDetails> querySupport(VkSurfaceKHR surface) noexcept
    {
        const auto physicalDevice = vkContext->getPhysicalDevice(); 

        auto details = std::make_shared<SwapChainSupportDetails>();
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details->capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, VK_NULL_HANDLE);

        details->surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details->surfaceFormats.data());
        
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, VK_NULL_HANDLE);

        details->presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details->presentModes.data());
        
        return details;
    }


    static VkExtent2D chooseSwapExtent(const std::shared_ptr<SwapChainSupportDetails> details, VkExtent2D currentExtent) noexcept
    {
        VkExtent2D actualExtent = { 0, 0 };

        if (details->capabilities.currentExtent.width != UINT_MAX)
        {
            return details->capabilities.currentExtent;
        }
        else
        {
            actualExtent = currentExtent;
            actualExtent.width  = glm_clamp(actualExtent.width, details->capabilities.minImageExtent.width, details->capabilities.maxImageExtent.width);
            actualExtent.height = glm_clamp(actualExtent.height, details->capabilities.minImageExtent.height, details->capabilities.maxImageExtent.height);
        }

        return actualExtent;
    }


    VkPresentModeKHR getPresentMode() const noexcept
    {
        for (const auto mode : presentModes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;

        return VK_PRESENT_MODE_FIFO_KHR;
    }


    VkSurfaceFormatKHR getSurfaceFormat() const noexcept
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




Swapchain::Swapchain(VkSurfaceKHR surface) noexcept:
    m_surface(surface),
    m_handle(VK_NULL_HANDLE),
    m_depthBuffer{}
{

}


bool Swapchain::create() noexcept
{
    auto context = vkContext;
    VkDevice device = context->getLogicalDevice();

    VkSwapchainKHR oldSwapchain = m_handle;

    if (m_handle)
    {
        for (const auto& attachment : m_colorAttachments)
            vkDestroyImageView(device, attachment.imageView, VK_NULL_HANDLE);

        m_handle = VK_NULL_HANDLE;
    }

    auto swapChainSupportDetails = SwapChainSupportDetails::querySupport(m_surface);
    const uint32_t minImageCount = swapChainSupportDetails->capabilities.minImageCount;
    const auto imageFormat = swapChainSupportDetails->getSurfaceFormat().format;
    m_extent = SwapChainSupportDetails::chooseSwapExtent(swapChainSupportDetails, m_extent);

    const VkSwapchainCreateInfoKHR swapchainInfo = 
    {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = VK_NULL_HANDLE,
        .flags                 = 0,
        .surface               = m_surface,
        .minImageCount         = minImageCount,
        .imageFormat           = imageFormat,
        .imageColorSpace       = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent           = m_extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = VK_NULL_HANDLE,
        .preTransform          = swapChainSupportDetails->capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = swapChainSupportDetails->getPresentMode(),
        .clipped               = VK_TRUE,
        .oldSwapchain          = oldSwapchain
    };

    if (vkCreateSwapchainKHR(device, &swapchainInfo, VK_NULL_HANDLE, &m_handle) == VK_SUCCESS)
    {
        if (oldSwapchain)
            vkDestroySwapchainKHR(device, oldSwapchain, VK_NULL_HANDLE);

        uint32_t imageCount = 0;
        
        if (vkGetSwapchainImagesKHR(device, m_handle, &imageCount, VK_NULL_HANDLE) != VK_SUCCESS)
            return false;

        auto& attachments = m_colorAttachments;

        if (attachments.empty())
            attachments.resize(imageCount);
        
        std::vector<VkImage> newImages(imageCount);
        
        if (vkGetSwapchainImagesKHR(device, m_handle, &imageCount, newImages.data()) == VK_SUCCESS)
        {
            for (uint32_t i = 0; i < attachments.size(); ++i)
            {
                attachments[i].image = newImages[i];
                attachments[i].format = imageFormat;
                
                if (auto imageView = vktools::create_image_view_2D(newImages[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT))
                    attachments[i].imageView = imageView;
                else
                    return false;  
            }

//          Depth buffering
            if (m_depthBuffer.attachment.imageView)
                vkDestroyImageView(device, m_depthBuffer.attachment.imageView, VK_NULL_HANDLE);

            if (m_depthBuffer.attachment.image)
                vkDestroyImage(device, m_depthBuffer.attachment.image, VK_NULL_HANDLE);

            if (m_depthBuffer.memory)
                vkFreeMemory(device, m_depthBuffer.memory, VK_NULL_HANDLE);

            if (m_depthBuffer.attachment.format == VK_FORMAT_UNDEFINED)
            {
                constexpr std::array<VkFormat, 3> formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
                m_depthBuffer.attachment.format = vktools::find_supported_format(formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, context->getPhysicalDevice());
            }

            if (m_depthBuffer.attachment.format != VK_FORMAT_UNDEFINED)
            {
                m_depthBuffer.attachment.image = vktools::create_image_2D(m_extent, 
                                                                          m_depthBuffer.attachment.format, 
                                                                          VK_IMAGE_TILING_OPTIMAL, 
                                                                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                                                          &m_depthBuffer.memory);

                if (m_depthBuffer.attachment.image)
                    m_depthBuffer.attachment.imageView = vktools::create_image_view_2D(m_depthBuffer.attachment.image, m_depthBuffer.attachment.format, VK_IMAGE_ASPECT_DEPTH_BIT);
                
                if (m_depthBuffer.attachment.imageView)
                    return true;
            }

            return true;
        }
    }

    return false;
}


void Swapchain::destroy() noexcept
{
    VkDevice device = vkContext->getLogicalDevice();

    if(m_handle)
    {
        for(const auto& attachment : m_colorAttachments)
            vkDestroyImageView(device, attachment.imageView, VK_NULL_HANDLE);

        vkDestroySwapchainKHR(device, m_handle, VK_NULL_HANDLE);
    }

    if (m_depthBuffer.attachment.imageView)
        vkDestroyImageView(device, m_depthBuffer.attachment.imageView, VK_NULL_HANDLE);

    if (m_depthBuffer.attachment.image)
        vkDestroyImage(device, m_depthBuffer.attachment.image, VK_NULL_HANDLE);

    if (m_depthBuffer.memory)
        vkFreeMemory(device, m_depthBuffer.memory, VK_NULL_HANDLE);
}


const VkSwapchainKHR& Swapchain::getHandle() const noexcept
{
    return m_handle;
}


const Swapchain::Attachment& Swapchain::getColorAttachment(size_t index) const noexcept
{
    return m_colorAttachments[index];
}


const Swapchain::Attachment& Swapchain::getDepthAttachment() const noexcept
{
    return m_depthBuffer.attachment;
}


size_t Swapchain::getImageCount() const noexcept
{
    return m_colorAttachments.size();
}


VkExtent2D Swapchain::getSize() const noexcept
{
    return m_extent;
}