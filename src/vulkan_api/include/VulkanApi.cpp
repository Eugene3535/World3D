#include "camera/Camera.hpp"
#include "app/Application.hpp"

#include "VulkanApi.hpp"


VulkanApi::VulkanApi() noexcept
{

}


VulkanApi::~VulkanApi()
{
    if(m_api)
        std::static_pointer_cast<VulkanApp>(m_api)->destroy();
}



bool VulkanApi::create(const char* title, int width, int height) noexcept
{
    if(m_api)
        return true;

    auto api = std::make_shared<VulkanApp>();

    if(api->create(title, width, height))
    {
        m_api = std::static_pointer_cast<void>(api);

        return true;
    }

    return false;
}


int VulkanApi::run() noexcept
{
    int retCode = -1;

    if(m_api)
        retCode = std::static_pointer_cast<VulkanApp>(m_api)->run();

    return retCode;
}