#ifndef VULKAN_API_HPP
#define VULKAN_API_HPP

#include <memory>

#include "Export.hpp"


class VK_API VulkanApi final
{
public:
    VulkanApi() noexcept;
    ~VulkanApi();

    bool create(const char* title, int width, int height) noexcept;
    int run() noexcept;

private:
    std::shared_ptr<void> m_engine;
};

#endif // !VULKAN_API_HPP