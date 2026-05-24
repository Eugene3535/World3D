#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <vulkan/vulkan.h>


class Surface
{
public:
    Surface() noexcept;

    bool create(VkInstance instance, uint64_t windowHandle) noexcept;
    void destroy(VkInstance instance) noexcept;

    VkSurfaceKHR handle;
};

#endif // !SURFACE_HPP