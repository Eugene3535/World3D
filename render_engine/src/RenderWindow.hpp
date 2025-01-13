#ifndef RENDER_WINDOW_HPP
#define RENDER_WINDOW_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Export.hpp"

class RE_API RenderWindow final
{
public:
	enum class GlOption
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

	RenderWindow() noexcept;
	~RenderWindow() noexcept;

	void setCursorPosition(int x, int y) noexcept;

	glm::i32vec2 getCursorPosition() const noexcept;
    glm::i32vec2 getPosition() const noexcept;
    glm::i32vec2 getSize() const noexcept;

	void addScene(std::unique_ptr<class Scene>&& scene) noexcept;

	void enable(GlOption option) noexcept;
	void disable(GlOption option) noexcept;

	void setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept;

	int run(void(*)(void*, int, int)) noexcept;

	void draw() const noexcept;
	void close() noexcept;

	bool isOpen() const noexcept;

	void* getNativeHandle() noexcept;

private:
	std::vector<std::unique_ptr<class Scene>> m_scenes;

	float m_clearColor[4];

	void* m_handle;
};

#endif // !RENDER_WINDOW_HPP
