#ifndef RENDER_WINDOW_HPP
#define RENDER_WINDOW_HPP

#include <memory>
#include <vector>

#include "window/BaseWindow.hpp"

class OGL_API RenderWindow final: 
	public BaseWindow
{
public:
	RenderWindow(const char* title, int32_t width, int32_t height) noexcept;
	~RenderWindow() noexcept;

	void addScene(std::unique_ptr<class Scene>&& scene) noexcept;
	void setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept;
	void draw() const noexcept;

private:
	std::vector<std::unique_ptr<class Scene>> m_scenes;

	float m_clearColor[4];
};

#endif // !RENDER_WINDOW_HPP
