#include "surface/Surface.hpp"


Surface::Surface() noexcept:
    handle(nullptr)
{

}


bool Surface::create(VkInstance instance, uint64_t windowHandle) noexcept
{
    if (handle)
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

    return (vkCreateWin32SurfaceKHR(instance, &surfaceInfo, VK_NULL_HANDLE, &handle) == VK_SUCCESS);
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

    return (vkCreateXcbSurfaceKHR(instance, &surfaceInfo, VK_NULL_HANDLE, &handle) == VK_SUCCESS);
#endif

    return false;
}


void Surface::destroy(VkInstance instance) noexcept
{
    if (handle)
    {
        vkDestroySurfaceKHR(instance, handle, VK_NULL_HANDLE);
        handle = nullptr;
    }
}