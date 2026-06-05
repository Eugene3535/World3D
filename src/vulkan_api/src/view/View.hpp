#ifndef VIEW_HPP
#define VIEW_HPP

#include <memory>

#include <vulkan/vulkan.h>


class View final
{
public:
    View() noexcept;
    ~View();

    bool create(uint64_t windowHandle) noexcept;
    void destroy() noexcept;
    void resize() noexcept;

    VkSurfaceKHR    getSurface()   const noexcept;
    VkSwapchainKHR& getSwapchain() const noexcept;

    VkImage     getImage(size_t index)     const noexcept;
    VkImageView getImageView(size_t index) const noexcept;
    
    VkImage     getDepthImage()     const noexcept;
    VkImageView getDepthImageView() const noexcept;

    size_t getImageCount() const noexcept;

    VkExtent2D getSize()        const noexcept;
    VkFormat   getImageFormat() const noexcept;
    VkFormat   getDepthFormat() const noexcept;

    static View* getInstance() noexcept;

private:
    std::shared_ptr<void> m_data;
};

#define vkView View::getInstance()

#endif // !VIEW_HPP