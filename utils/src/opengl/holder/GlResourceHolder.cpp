#include "opengl/holder/GlResourceHolder.hpp"


GlResourceHolder::GlResourceHolder() noexcept:
    m_isLoaded(false)
{

}


GlResourceHolder::~GlResourceHolder() noexcept
{

}


bool GlResourceHolder::isLoaded() noexcept
{
    if(!m_isLoaded)
        m_isLoaded = gladLoadGL();

    return m_isLoaded;
}