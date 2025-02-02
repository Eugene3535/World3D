#ifndef BASE_WINDOW_HPP
#define BASE_WINDOW_HPP

#include <queue>

#include <glm/glm.hpp>

#include "opengl/context/GlContext.hpp"
#include "window/Input.hpp"

class OGL_API BaseWindow
{
public:
    BaseWindow(const char* title, int32_t width, int32_t height) noexcept;
    BaseWindow(const BaseWindow&) noexcept = delete;
    BaseWindow(BaseWindow&&) noexcept = delete;
    BaseWindow& operator = (const BaseWindow&) noexcept = delete;
    BaseWindow& operator = (BaseWindow&&) noexcept = delete;
    virtual ~BaseWindow() noexcept;

    bool isOpen() const noexcept;
    void close() noexcept;

    bool isKeyPressed(Keyboard::Key key) const noexcept;

    void setVerticalSyncEnabled(bool enabled) noexcept;

    void         setCursorPosition(int32_t x, int32_t y) noexcept;
	glm::i32vec2 getCursorPosition() const noexcept;

    void hideCursor() noexcept;
    void showCursor() noexcept;

    glm::i32vec2 getPosition() const noexcept;
    glm::i32vec2 getSize() const noexcept;

protected:
    void* m_handle;

    GlContext m_context;
};

#endif // !BASE_WINDOW_HPP