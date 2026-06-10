#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>


class Swapchain
{
public:
    struct Attachment
    {
        VkImage     image;
        VkImageView imageView;
        VkFormat    format;
    };

    Swapchain(VkSurfaceKHR surface) noexcept;

    bool create() noexcept;
    void destroy() noexcept;

    const VkSwapchainKHR& getHandle()                      const noexcept;
    const Attachment&     getColorAttachment(size_t index) const noexcept;
    const Attachment&     getDepthAttachment()             const noexcept;

    size_t getImageCount() const noexcept;
    VkExtent2D getSize() const noexcept;

private:
    VkSurfaceKHR   m_surface;
    VkSwapchainKHR m_handle;

    std::vector<Attachment> m_colorAttachments;
    struct
    {
        Attachment     attachment;
        VkDeviceMemory memory = nullptr;
    } m_depthBuffer;
    
    VkExtent2D m_extent;
};

#endif // !SWAPCHAIN_HPP