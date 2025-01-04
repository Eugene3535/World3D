#ifndef RENDER_WINDOW_HPP
#define RENDER_WINDOW_HPP


class RenderWindow
{
public:
	RenderWindow() noexcept;
	~RenderWindow() noexcept;

	int run(void(*)(struct GLFWwindow*, int, int)) noexcept;

private:
	struct GLFWwindow* m_window;
};

#endif // !RENDER_WINDOW_HPP
