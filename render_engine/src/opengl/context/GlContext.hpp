#ifndef GLCONTEXT_HPP
#define GLCONTEXT_HPP

#include "opengl/resources/buffers/GlBuffer.hpp"
#include "opengl/resources/shared/VertexArrayObject.hpp"
#include "opengl/resources/shared/Texture2D.hpp"

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

    bool isLoaded() noexcept;

    void enable(Option option) noexcept;
    void disable(Option option) noexcept;

	template<class T, size_t N>
    std::array<uint32_t, N> create() noexcept;
    
    template<class T, size_t N>
    void destroy(const std::array<uint32_t, N>& buffers) noexcept;

private:
    template<size_t N>
    std::array<uint32_t, N> createResources(std::vector<uint32_t>& handles, void(*)(int32_t, uint32_t*)) noexcept;
    
    template<size_t N>
    void destroyResources(const std::array<uint32_t, N>& objects, std::vector<uint32_t>& handles, void(*)(int32_t, const uint32_t*)) noexcept;

    static GlContext* m_instance;

    std::vector<uint32_t> m_buffers;
    std::vector<uint32_t> m_arrays;
    std::vector<uint32_t> m_textures;

    void(*genBuffers)(int32_t, uint32_t*);
    void(*delBuffers)(int32_t, const uint32_t*);

    void(*genVertexArrays)(int32_t, uint32_t*);
    void(*delVertexArrays)(int32_t, const uint32_t*);

    void(*genTextures)(int32_t, uint32_t*);
    void(*delTextures)(int32_t, const uint32_t*);

    bool m_isLoaded;
};

#include "opengl/context/GlContext.inl"

#define Context GlContext::getContext()

#endif