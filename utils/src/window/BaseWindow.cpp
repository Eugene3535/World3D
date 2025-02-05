#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/BaseWindow.hpp"


BaseWindow::BaseWindow(const char* title, int32_t width, int32_t height) noexcept:
    m_handle(nullptr)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    if (GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr); window != nullptr)
    {
        glfwMakeContextCurrent(window);

        if (m_resourceHolder.isLoaded())
        {
            m_handle = static_cast<void*>(window);

//  Setup callbacks
            glfwSetWindowUserPointer(window, static_cast<void*>(this));


            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
            {
                glViewport(0, 0, width, height);
            });
        }
        else
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }
}


BaseWindow::~BaseWindow() noexcept
{
    if(m_handle)
        glfwDestroyWindow(static_cast<GLFWwindow*>(m_handle));
    glfwTerminate();
}


bool BaseWindow::isOpen() const noexcept
{
    return (m_handle != nullptr) && !glfwWindowShouldClose(static_cast<GLFWwindow*>(m_handle));
}


void BaseWindow::close() noexcept
{
    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_handle), GLFW_TRUE);
}


bool BaseWindow::isKeyPressed(Keyboard::Key key) const noexcept
{
    auto keyToGLFWenum = [](Keyboard::Key key) -> int
    {
        switch (key)
        {
            case Keyboard::A:          return GLFW_KEY_A; 
            case Keyboard::B:          return GLFW_KEY_B; 
            case Keyboard::C:          return GLFW_KEY_C; 
            case Keyboard::D:          return GLFW_KEY_D; 
            case Keyboard::E:          return GLFW_KEY_E; 
            case Keyboard::F:          return GLFW_KEY_F; 
            case Keyboard::G:          return GLFW_KEY_G; 
            case Keyboard::H:          return GLFW_KEY_H; 
            case Keyboard::I:          return GLFW_KEY_I; 
            case Keyboard::J:          return GLFW_KEY_J; 
            case Keyboard::K:          return GLFW_KEY_K; 
            case Keyboard::L:          return GLFW_KEY_L; 
            case Keyboard::M:          return GLFW_KEY_M; 
            case Keyboard::N:          return GLFW_KEY_N; 
            case Keyboard::O:          return GLFW_KEY_O; 
            case Keyboard::P:          return GLFW_KEY_P; 
            case Keyboard::Q:          return GLFW_KEY_Q; 
            case Keyboard::R:          return GLFW_KEY_R; 
            case Keyboard::S:          return GLFW_KEY_S; 
            case Keyboard::T:          return GLFW_KEY_T; 
            case Keyboard::U:          return GLFW_KEY_U; 
            case Keyboard::V:          return GLFW_KEY_V; 
            case Keyboard::W:          return GLFW_KEY_W; 
            case Keyboard::X:          return GLFW_KEY_X; 
            case Keyboard::Y:          return GLFW_KEY_Y; 
            case Keyboard::Z:          return GLFW_KEY_Z;  
            case Keyboard::Num0:       return 0;
            case Keyboard::Num1:       return 0;
            case Keyboard::Num2:       return 0;
            case Keyboard::Num3:       return 0;
            case Keyboard::Num4:       return 0;
            case Keyboard::Num5:       return 0;
            case Keyboard::Num6:       return 0;
            case Keyboard::Num7:       return 0;
            case Keyboard::Num8:       return 0;
            case Keyboard::Num9:       return 0;
            case Keyboard::Escape:     return GLFW_KEY_ESCAPE;
            case Keyboard::LControl:   return 0;
            case Keyboard::LShift:     return 0;
            case Keyboard::LAlt:       return 0;
            case Keyboard::LSystem:    return 0; 
            case Keyboard::RControl:   return 0;
            case Keyboard::RShift:     return 0;  
            case Keyboard::RAlt:       return 0;  
            case Keyboard::RSystem:    return 0;
            case Keyboard::Menu:       return 0;
            case Keyboard::LBracket:   return 0;
            case Keyboard::RBracket:   return 0;
            case Keyboard::Semicolon:  return 0;
            case Keyboard::Comma:      return 0;
            case Keyboard::Period:     return 0;
            case Keyboard::Apostrophe: return 0;   
            case Keyboard::Slash:      return 0;        
            case Keyboard::Backslash:  return 0;    
            case Keyboard::Grave:      return 0;        
            case Keyboard::Equal:      return 0;        
            case Keyboard::Hyphen:     return 0;       
            case Keyboard::Space:      return GLFW_KEY_SPACE;        
            case Keyboard::Enter:      return GLFW_KEY_ENTER;      
            case Keyboard::Backspace:  return 0;    
            case Keyboard::Tab:        return 0;          
            case Keyboard::PageUp:     return 0;       
            case Keyboard::PageDown:   return 0;     
            case Keyboard::End:        return 0;          
            case Keyboard::Home:       return 0;         
            case Keyboard::Insert:     return 0;       
            case Keyboard::Delete:     return 0;       
            case Keyboard::Add:        return 0;          
            case Keyboard::Subtract:   return 0;
            case Keyboard::Multiply:   return 0;     
            case Keyboard::Divide:     return 0;    
            case Keyboard::Left:       return GLFW_KEY_LEFT;    
            case Keyboard::Right:      return GLFW_KEY_RIGHT;    
            case Keyboard::Up:         return GLFW_KEY_UP;    
            case Keyboard::Down:       return GLFW_KEY_DOWN;   
            case Keyboard::Numpad0:    return 0;
            case Keyboard::Numpad1:    return 0;
            case Keyboard::Numpad2:    return 0;
            case Keyboard::Numpad3:    return 0;
            case Keyboard::Numpad4:    return 0;
            case Keyboard::Numpad5:    return 0;
            case Keyboard::Numpad6:    return 0;
            case Keyboard::Numpad7:    return 0;
            case Keyboard::Numpad8:    return 0;
            case Keyboard::Numpad9:    return 0;
            case Keyboard::F1:         return GLFW_KEY_F1;
            case Keyboard::F2:         return GLFW_KEY_F2;
            case Keyboard::F3:         return GLFW_KEY_F3;
            case Keyboard::F4:         return GLFW_KEY_F4;
            case Keyboard::F5:         return GLFW_KEY_F5;
            case Keyboard::F6:         return GLFW_KEY_F6;
            case Keyboard::F7:         return GLFW_KEY_F7;
            case Keyboard::F8:         return GLFW_KEY_F8;
            case Keyboard::F9:         return GLFW_KEY_F9;
            case Keyboard::F10:        return GLFW_KEY_F10;
            case Keyboard::F11:        return GLFW_KEY_F11;
            case Keyboard::F12:        return GLFW_KEY_F12;
            case Keyboard::F13:        return GLFW_KEY_F13;
            case Keyboard::F14:        return GLFW_KEY_F14;
            case Keyboard::F15:        return GLFW_KEY_F15;
            case Keyboard::Pause:      return 0;
        
            default: return 0;
        }
    };

    return glfwGetKey(static_cast<GLFWwindow*>(m_handle), keyToGLFWenum(key)) == GLFW_PRESS;
}


void BaseWindow::setVerticalSyncEnabled(bool enabled) noexcept
{
    glfwSwapInterval(enabled ? 1 : 0);
}


void BaseWindow::setCursorPosition(int32_t x, int32_t y) noexcept
{
    glfwSetCursorPos(static_cast<GLFWwindow*>(m_handle), x, y);
}


glm::i32vec2 BaseWindow::getCursorPosition() const noexcept
{
    double xpos, ypos;
    glfwGetCursorPos(static_cast<GLFWwindow*>(m_handle), &xpos, &ypos);

    return { static_cast<int32_t>(xpos), static_cast<int32_t>(ypos) };
}


void BaseWindow::hideCursor() noexcept
{
    glfwSetInputMode(static_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void BaseWindow::showCursor() noexcept
{
    glfwSetInputMode(static_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}


glm::i32vec2 BaseWindow::getPosition() const noexcept
{
    int32_t xt, yt;
    glfwGetWindowPos(static_cast<GLFWwindow*>(m_handle), &xt, &yt);

    return { static_cast<int32_t>(xt), static_cast<int32_t>(yt) };
}


glm::i32vec2 BaseWindow::getSize() const noexcept
{
    int32_t width, height;
    glfwGetWindowSize(static_cast<GLFWwindow*>(m_handle), &width, &height);

    return { static_cast<int32_t>(width), static_cast<int32_t>(height) };
}


GlResourceHolder* BaseWindow::getResourceHolder() noexcept
{
    return &m_resourceHolder;
}