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

    VkSurfaceKHR           getSurface()   const noexcept;
    const class Swapchain* getSwapchain() const noexcept; 

    static View* getInstance() noexcept;

private:
    bool createSurface(uint64_t windowHandle) noexcept;

    VkSurfaceKHR m_surface;
    std::unique_ptr<class Swapchain> m_swapchain;
};

#define vkView View::getInstance()

#endif // !VIEW_HPP