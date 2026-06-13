#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>

#include "context/Context.hpp"
#include "view/View.hpp"
#include "sync/SyncManager.hpp"
#include "scenes/root/RootScene.hpp"
#include "render/Renderer.hpp"
#include "camera/Camera.hpp"
#include "resources/ResourceManager.hpp"


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

    std::unique_ptr<RootScene> m_rootScene;

    Renderer m_renderer;

    bool    m_framebufferResized;
    int32_t m_width;
    int32_t m_height;

    Camera camera;

    ResourceManager m_resources;
};

#endif // !ENGINE_HPP