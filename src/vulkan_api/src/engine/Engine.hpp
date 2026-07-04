#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>

#include "context/Context.hpp"
#include "view/View.hpp"
#include "sync/SyncManager.hpp"
#include "scenes/root/RootScene.hpp"
#include "render/RenderTarget.hpp"
#include "camera/Camera.hpp"


class Engine
{
public:
    Engine() noexcept;

    bool createContext()                       noexcept;
    bool createMainView(uint64_t windowHandle) noexcept;

    bool createPipeline() noexcept;
    void drawFrame() noexcept;
    void destroy() noexcept;
    void resize(int width, int height) noexcept;

    VulkanContext m_context;
    View          m_view;
    SyncManager   m_sync;

    VkCommandPool m_commandPool;
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> m_commandBuffers;

    std::unique_ptr<RootScene> m_rootScene;

    RenderTarget m_renderer;

    bool    m_framebufferResized;
    int32_t m_width;
    int32_t m_height;

    Camera camera;
};

#endif // !ENGINE_HPP