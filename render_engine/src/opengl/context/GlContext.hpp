#ifndef GLCONTEXT_HPP
#define GLCONTEXT_HPP

#include "opengl/resources/GlResourceHolder.hpp"

class RE_API GlContext final
{
public:
	enum class Option
	{
		Blend,
		ClipDistancei,
		ColorLogicOp,
		CullFace,
		DepthClamp,
		DepthTest,
		Dither,
		FramebufferSrgb,
		LineSmooth,
		Multisample,
		PolygonOffsetFill,
		PolygonOffsetLine,
		PolygonOffsetPoint,
		PolygonSmooth,
		PrimitiveRestart,
		SampleAlphaToCoverage,
		SampleAlphaToOne,
		SampleCoverage,
		ScissorTest,
		StencilTest,
		TextureCubeMapSeamless,
		ProgramPointSize
	};

    GlContext() noexcept;
    ~GlContext() noexcept;

    static GlContext* getContext() noexcept;
	GlResourceHolder* getGlResourceHolder() noexcept;

    bool isLoaded() noexcept;

    void enable(Option option) noexcept;
    void disable(Option option) noexcept;

private:
    static GlContext* m_instance;

	GlResourceHolder m_bufferHolder;

    bool m_isLoaded;
};

#define Context GlContext::getContext()

#endif