#ifndef VULKAN_APP_HPP
#define VULKAN_APP_HPP

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "VulkanApi.hpp"


class MainWindow
{
public:
    MainWindow() noexcept;
    ~MainWindow();

    bool create(const char* title, int32_t width, int32_t height) noexcept;
    int run() noexcept;

private:
    void initCallbacks() noexcept;

    GLFWwindow* m_window;
	VulkanApi m_api;
};

#endif // !VULKAN_APP_HPP