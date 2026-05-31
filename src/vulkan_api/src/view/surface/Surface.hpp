#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <vulkan/vulkan.h>


class Surface
{
public:
    Surface() noexcept;

    bool create(uint64_t windowHandle) noexcept;
    void destroy() noexcept;

    VkSurfaceKHR handle;
};

#endif // !SURFACE_HPP