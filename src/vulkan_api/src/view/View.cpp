#include <cassert>

#include "context/Context.hpp"
#include "view/swapchain/Swapchain.hpp"
#include "view/View.hpp"


static View* g_view;


View::View() noexcept:
    m_surface(VK_NULL_HANDLE)
{
    assert(g_view == nullptr);
    g_view = this;
}


View::~View() = default;


bool View::create(uint64_t windowHandle) noexcept
{
    if (m_surface && m_swapchain)
        return true;

    if (!createSurface(windowHandle))
        return false;

    auto swapchain = std::make_unique<Swapchain>(m_surface);

    if (!swapchain->create())
        return false;

    swapchain.swap(m_swapchain);

    return true;
}


void View::destroy() noexcept
{
    if (m_swapchain)
        m_swapchain->destroy();

    vkDestroySurfaceKHR(vkContext->get<VkInstance>(), m_surface, VK_NULL_HANDLE);
}


void View::resize() noexcept
{
    if (m_swapchain)
        m_swapchain->create();
}


VkSurfaceKHR View::getSurface() const noexcept
{
    return m_surface;
}


const Swapchain* View::getSwapchain() const noexcept
{   
    return m_swapchain.get();
}


View* View::getInstance() noexcept
{
    return g_view;
}


bool View::createSurface(uint64_t windowHandle) noexcept
{
    if (m_surface)
        return true;

    auto instance = vkContext->get<VkInstance>();

#ifdef _WIN32
    const VkWin32SurfaceCreateInfoKHR surfaceInfo = 
    {
        .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext     = VK_NULL_HANDLE,
        .flags     = 0,
        .hinstance = GetModuleHandle(VK_NULL_HANDLE),
        .hwnd      = reinterpret_cast<HWND>(windowHandle)
    };

    return (vkCreateWin32SurfaceKHR(instance, &surfaceInfo, VK_NULL_HANDLE, &m_surface) == VK_SUCCESS);
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

    return (vkCreateXcbSurfaceKHR(instance, &surfaceInfo, VK_NULL_HANDLE, &m_surface) == VK_SUCCESS);
#endif

    return false;
}