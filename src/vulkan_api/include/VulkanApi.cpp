#include "engine/Engine.hpp"
#include "VulkanApi.hpp"


VulkanApi::VulkanApi() noexcept
{

}


VulkanApi::~VulkanApi()
{
    if(m_engine)
        std::static_pointer_cast<Engine>(m_engine)->destroy();
}


bool VulkanApi::init(uint64_t windowHandle) noexcept
{
    auto engine = std::make_shared<Engine>();

    if(engine->init(windowHandle))
    {
        m_engine = std::static_pointer_cast<void>(engine);

        return true;
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
