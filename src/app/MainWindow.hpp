#ifndef VULKAN_APP_HPP
#define VULKAN_APP_HPP

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

    struct GLFWwindow* m_window;
	VulkanApi m_api;
};

#endif // !VULKAN_APP_HPP