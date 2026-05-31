#include <cassert>

#include "view/surface/Surface.hpp"
#include "view/swapchain/Swapchain.hpp"
#include "view/View.hpp"


static View* g_view;

struct ViewData
{
    Surface surface;
    Swapchain swapchain;
};


View::View() noexcept
{
    assert(g_view == nullptr);
    g_view = this;
}


View::~View()
{

}


bool View::create(uint64_t windowHandle) noexcept
{
    if (m_data)
        return true;

    auto data = std::make_shared<ViewData>();

    if (!data->surface.create(windowHandle))
        return false;

    if (!data->swapchain.create(data->surface.handle))
        return false;

    m_data = data;

    return true;
}


void View::destroy() noexcept
{
    if (m_data)
    {
        std::static_pointer_cast<ViewData>(m_data)->swapchain.destroy();
        std::static_pointer_cast<ViewData>(m_data)->surface.destroy();
        m_data.reset();
    }
}


void View::resize() noexcept
{
    std::static_pointer_cast<ViewData>(m_data)->swapchain.create(getSurface());
}


VkSurfaceKHR View::getSurface() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->surface.handle;
}


VkSwapchainKHR& View::getSwapchain() const noexcept
{   
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.handle;
}


VkImage View::getImage(size_t index) const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.images[index];
}


VkImageView View::getImageView(size_t index) const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.imageViews[index];
}


VkImage View::getDepthImage() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.depthBuffer.image;
}


VkImageView View::getDepthImageView() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.depthBuffer.imageView;
}


VkExtent2D View::getSize() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.extent;
}


VkFormat View::getImageFormat() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.imageFormat;
}


VkFormat View::getDepthFormat() const noexcept
{
    return std::static_pointer_cast<ViewData>(m_data)->swapchain.depthBuffer.format;
}


View* View::getInstance() noexcept
{
    return g_view;
}