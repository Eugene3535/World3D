#ifndef BASE_WINDOW_HPP
#define BASE_WINDOW_HPP

#include <string_view>

#include <glm/glm.hpp>

#include "opengl/context/GlContext.hpp"

class RE_API BaseWindow
{
public:
    BaseWindow(std::string_view title, int32_t width, int32_t height) noexcept;
    BaseWindow(const BaseWindow&) noexcept = delete;
    BaseWindow(BaseWindow&&) noexcept = delete;
    BaseWindow& operator = (const BaseWindow&) noexcept = delete;
    BaseWindow& operator = (BaseWindow&&) noexcept = delete;
    virtual ~BaseWindow() noexcept;

    void close() noexcept;

    void setVerticalSyncEnabled(bool enabled) noexcept;

    void setCursorPosition(int32_t x, int32_t y) noexcept;

	glm::i32vec2 getCursorPosition() const noexcept;
    glm::i32vec2 getPosition() const noexcept;
    glm::i32vec2 getSize() const noexcept;

    void* getGLFWHandle() noexcept;

    bool isOpen() const noexcept;

protected:
    void* m_handle;

    GlContext m_context;
};

#endif // !BASE_WINDOW_HPP