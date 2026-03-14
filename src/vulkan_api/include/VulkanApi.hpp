#ifndef VULKAN_API_HPP
#define VULKAN_API_HPP

#include <memory>

#include "Export.hpp"


class VK_API VulkanApi final
{
public:
    VulkanApi() noexcept;
    ~VulkanApi();

    bool createContext() noexcept;
    bool createMainView(uint64_t windowHandle) noexcept;

    bool init() noexcept;

    void drawFrame() const noexcept;

    void processMouseMovement(float xpos, float ypos) const noexcept;
    void processKeyboard(int direction, float deltaTime) const noexcept;

    void resize(int width, int height) const noexcept;

private:
    std::shared_ptr<void> m_engine;
};

#endif // !VULKAN_API_HPP