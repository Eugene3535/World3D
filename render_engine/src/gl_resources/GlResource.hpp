#ifndef GL_RESOURCE_HPP
#define GL_RESOURCE_HPP

#include <cstdint>

#include "Export.hpp"


class RE_API GlResource
{
public:
    GlResource()                               noexcept;
    GlResource(const GlResource&)              noexcept = delete;
    GlResource(GlResource&&)                   noexcept;
    GlResource& operator = (const GlResource&) noexcept = delete;
    GlResource& operator = (GlResource&&)      noexcept;
    virtual ~GlResource()                      noexcept;

    uint32_t getHandle() const noexcept;
    bool isValid() const noexcept;

protected:
    uint32_t m_handle;
};

#endif // !GL_RESOURCE_HPP