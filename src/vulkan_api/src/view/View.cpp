#include <cassert>

#include "context/Context.hpp"
#include "resources/ResourceManager.hpp"
#include "view/swapchain/Swapchain.hpp"
#include "view/View.hpp"


static View* g_view;

struct ViewData
{
    Swapchain swapchain;
};


View::View() noexcept:
    m_surface(VK_NULL_HANDLE)
{
    assert(g_view == nullptr);
    g_view = this;
}


View::~View() = default;


bool View::create(uint64_t windowHandle) noexcept
{
    if (m_data)
        return true;

    auto data = std::make_shared<ViewData>();

    if (!createSurface(windowHandle))
        return false;

    if (!data->swapchain.create(m_surface))
        return false;

    m_data = data;

    return true;
}


void View::destroy() noexcept
{
    if (m_data)
    {
        std::static_pointer_cast<ViewData>(m_data)->swapchain.destroy();
        m_data.reset();
    }
}


void View::resize() noexcept
{
    std::static_pointer_cast<ViewData>(m_data)->swapchain.create(getSurface());
}


VkSurfaceKHR View::getSurface() const noexcept
{
    return m_surface;
}


VkSwapchainKHR& View::getSwapchain() const noexcept
{   
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.handle;
}


VkImage View::getImage(size_t index) const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.images[index];
}


VkImageView View::getImageView(size_t index) const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.imageViews[index];
}


VkImage View::getDepthImage() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.depthBuffer.image;
}


VkImageView View::getDepthImageView() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.depthBuffer.imageView;
}


size_t View::getImageCount() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.images.size();
}


VkExtent2D View::getSize() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.extent;
}


VkFormat View::getImageFormat() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.imageFormat;
}


VkFormat View::getDepthFormat() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.depthBuffer.format;
}


View* View::getInstance() noexcept
{
    return g_view;
}


bool View::createSurface(uint64_t windowHandle) noexcept
{
    if (m_surface)
        return true;

    VkInstance instance = vkContext->getInstance();

    return vkResource->allocateObject(VK_OBJECT_TYPE_SURFACE_KHR, [this, instance, windowHandle]() -> void*
    {
#ifdef _WIN32
        const VkWin32SurfaceCreateInfoKHR surfaceInfo = 
        {
            .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext     = VK_NULL_HANDLE,
            .flags     = 0,
            .hinstance = GetModuleHandle(VK_NULL_HANDLE),
            .hwnd      = reinterpret_cast<HWND>(windowHandle)
        };

        if (vkCreateWin32SurfaceKHR(instance, &surfaceInfo, VK_NULL_HANDLE, &m_surface) == VK_SUCCESS)
            return static_cast<void*>(m_surface);
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

        if (vkCreateXcbSurfaceKHR(instance, &surfaceInfo, VK_NULL_HANDLE, &m_surface) == VK_SUCCESS)
            return static_cast<void*>(m_surface);
#endif

        return VK_NULL_HANDLE;
    });
}