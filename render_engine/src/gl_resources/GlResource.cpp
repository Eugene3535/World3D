#include "gl_resources/GlResource.hpp"


GlResource::GlResource() noexcept:
    m_handle(0)
{

}


GlResource::GlResource(GlResource&& other) noexcept:
    m_handle(other.m_handle)
{
    other.m_handle = 0;
}


GlResource& GlResource::operator = (GlResource&& other) noexcept
{
    m_handle = other.m_handle;
    other.m_handle = 0;

    return *this;
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