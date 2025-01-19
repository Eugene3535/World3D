#include "opengl/resources/GlResource.hpp"


GlResource::GlResource(uint32_t handle) noexcept:
    m_handle(handle)
{

}


GlResource::~GlResource() noexcept = default; // <- glDelete<Resource>() implementation


uint32_t GlResource::getHandle() const noexcept
{
    return m_handle;
}


bool GlResource::isValid() const noexcept
{
    return m_handle != 0;
}