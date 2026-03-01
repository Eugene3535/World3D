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


bool VulkanApi::create(const char* title, int width, int height) noexcept
{
    if(m_engine)
        return true;

    auto engine = std::make_shared<Engine>();

    if(engine->create(title, width, height))
    {
        m_engine = std::static_pointer_cast<void>(engine);

        return true;
    }

    return false;
}


int VulkanApi::run() noexcept
{
    int retCode = -1;

    if(m_engine)
        retCode = std::static_pointer_cast<Engine>(m_engine)->run();

    return retCode;
}