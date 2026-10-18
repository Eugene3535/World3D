#include "VulkanApi.hpp"
#include "engine/Engine.hpp"


VulkanApi::VulkanApi(Camera& camera) noexcept : GraphicsApi(camera)
{
}


VulkanApi::~VulkanApi()
{
    if (m_engine)
        m_engine->destroy();
}


bool VulkanApi::createContext() noexcept
{
    if (m_engine)
        return true;

    auto engine = std::make_unique<Engine>(m_camera);

    if (engine->createContext())
    {
        m_engine.swap(engine);

        return true;
    }

    return false;
}


bool VulkanApi::createView(uint64_t windowHandle) noexcept
{
    if (m_engine)
    {
        if (m_engine->createView(windowHandle))
            return m_engine->createPipeline(); // tmp solution
    }

    return false;
}


void VulkanApi::drawFrame() const noexcept
{
    if (m_engine)
    {
        m_engine->drawFrame();
    }
}


void VulkanApi::resize(int width, int height) noexcept
{
    if (m_engine)
    {
        m_engine->resize(width, height);
    }
}
