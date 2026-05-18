#include "engine/Engine.hpp"
#include "VulkanApi.hpp"


VulkanApi::VulkanApi() noexcept
{

}


VulkanApi::~VulkanApi()
{
    if (m_engine)
        std::static_pointer_cast<Engine>(m_engine)->destroy();
}


bool VulkanApi::init() noexcept
{
    if (m_engine)
        return true;

    m_engine = std::make_shared<Engine>();

    return true;
}


bool VulkanApi::createContext() noexcept
{
    if (!m_engine)
        return true;

    auto engine = std::static_pointer_cast<Engine>(m_engine);

    return engine->createContext();
}


bool VulkanApi::createMainView(uint64_t windowHandle) noexcept
{
    if (m_engine)
    {
        auto engine = std::static_pointer_cast<Engine>(m_engine);

        if (engine->createMainView(windowHandle))
            return engine->createPipeline(); // tmp solution
    }

    return false;
}


void VulkanApi::drawFrame() const noexcept
{
    if (auto engine = std::static_pointer_cast<Engine>(m_engine))
    {
        engine->drawFrame();
    }
}


void VulkanApi::processMouseMovement(float xpos, float ypos) const noexcept
{
    if (auto engine = std::static_pointer_cast<Engine>(m_engine))
    {
        engine->camera.processMouseMovement(xpos, ypos);
    }
}


void VulkanApi::processKeyboard(int direction, float deltaTime) const noexcept
{
    if (auto engine = std::static_pointer_cast<Engine>(m_engine))
    {
        engine->camera.processKeyboard((Camera::Direction)direction, deltaTime);
    }
}


void VulkanApi::resize(int width, int height) const noexcept
{
    if (auto engine = std::static_pointer_cast<Engine>(m_engine))
    {
        engine->resize(width, height);
    }
}
