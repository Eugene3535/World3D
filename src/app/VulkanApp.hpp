#ifndef VULKAN_APP_HPP
#define VULKAN_APP_HPP

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "VulkanApi.hpp"


class VulkanApp
{
public:
    VulkanApp() noexcept;
    ~VulkanApp();

    bool init() noexcept;
    int run() noexcept;

private:
    bool createWindow() noexcept;

    GLFWwindow* m_window;
	VulkanApi m_api;
};

#endif // !VULKAN_APP_HPP