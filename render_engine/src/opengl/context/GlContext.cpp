#include <cassert>

#include <glad/glad.h>

#include "opengl/context/GlContext.hpp"


GlContext* GlContext::m_instance;


static constexpr uint32_t optionToGlCap(const GlContext::Option option) noexcept
{
    switch (option)
    {
        case GlContext::Option::Blend:                  return GL_BLEND;
        case GlContext::Option::ClipDistancei:          return GL_CLIP_DISTANCE0;
        case GlContext::Option::ColorLogicOp:           return GL_COLOR_LOGIC_OP;
        case GlContext::Option::CullFace:               return GL_CULL_FACE;
        case GlContext::Option::DepthClamp:             return GL_DEPTH_CLAMP;
        case GlContext::Option::DepthTest:              return GL_DEPTH_TEST;
        case GlContext::Option::Dither:                 return GL_DITHER;
        case GlContext::Option::FramebufferSrgb:        return GL_FRAMEBUFFER_SRGB;
        case GlContext::Option::LineSmooth:             return GL_LINE_SMOOTH;
        case GlContext::Option::Multisample:            return GL_MULTISAMPLE;
        case GlContext::Option::PolygonOffsetFill:      return GL_POLYGON_OFFSET_FILL;
        case GlContext::Option::PolygonOffsetLine:      return GL_POLYGON_OFFSET_LINE;
        case GlContext::Option::PolygonOffsetPoint:     return GL_POLYGON_OFFSET_POINT;
        case GlContext::Option::PolygonSmooth:          return GL_POLYGON_SMOOTH;
        case GlContext::Option::PrimitiveRestart:       return GL_PRIMITIVE_RESTART;
        case GlContext::Option::SampleAlphaToCoverage:  return GL_SAMPLE_ALPHA_TO_COVERAGE;
        case GlContext::Option::SampleAlphaToOne:       return GL_SAMPLE_ALPHA_TO_ONE;
        case GlContext::Option::SampleCoverage:         return GL_SAMPLE_COVERAGE;
        case GlContext::Option::ScissorTest:            return GL_SCISSOR_TEST;
        case GlContext::Option::StencilTest:            return GL_STENCIL_TEST;
        case GlContext::Option::TextureCubeMapSeamless: return GL_TEXTURE_CUBE_MAP_SEAMLESS;
        case GlContext::Option::ProgramPointSize:       return GL_PROGRAM_POINT_SIZE;

        default: return 0;
    }
}


GlContext::GlContext() noexcept:
    m_isLoaded(false)
{
    assert(m_instance == nullptr);
    m_instance = this;
}


GlContext::~GlContext() noexcept
{

}


GlContext* GlContext::getContext() noexcept
{
    return m_instance;
}


bool GlContext::isLoaded() noexcept
{
    if(!m_isLoaded)
        m_isLoaded = (gladLoadGL() != 0);

    return m_isLoaded;
}


void GlContext::enable(GlContext::Option option) noexcept
{
    glEnable(optionToGlCap(option));
}


void GlContext::disable(GlContext::Option option) noexcept
{
    glDisable(optionToGlCap(option));
}