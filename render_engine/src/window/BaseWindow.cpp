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

        if (Context->isLoaded())
        {
            m_handle = static_cast<void*>(window);

//  Setup callbacks
            glfwSetWindowUserPointer(window, static_cast<void*>(this));


            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
            {
                glViewport(0, 0, width, height);
            });


            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                auto base_window = static_cast<BaseWindow*>(glfwGetWindowUserPointer(window));
                Event event;
                 
                if (action == GLFW_PRESS)
                {
                    event.type = Event::EventType::KeyPressed;

                    switch (key)
                    {
                        case GLFW_KEY_SPACE:         break;          
                        case GLFW_KEY_APOSTROPHE:    break; /* ' */
                        case GLFW_KEY_COMMA:         break; /* , */
                        case GLFW_KEY_MINUS:         break; /* - */
                        case GLFW_KEY_PERIOD:        break; /* . */
                        case GLFW_KEY_SLASH:         break; /* / */
                        case GLFW_KEY_0:             break;              
                        case GLFW_KEY_1:             break;              
                        case GLFW_KEY_2:             break;              
                        case GLFW_KEY_3:             break;              
                        case GLFW_KEY_4:             break;              
                        case GLFW_KEY_5:             break;              
                        case GLFW_KEY_6:             break;              
                        case GLFW_KEY_7:             break;              
                        case GLFW_KEY_8:             break;              
                        case GLFW_KEY_9:             break;              
                        case GLFW_KEY_SEMICOLON:     break; /* ; */
                        case GLFW_KEY_EQUAL:         break; /* = */
                        case GLFW_KEY_A:             event.key.code = Keyboard::A; break;              
                        case GLFW_KEY_B:             break;              
                        case GLFW_KEY_C:             break;              
                        case GLFW_KEY_D:             event.key.code = Keyboard::D; break;              
                        case GLFW_KEY_E:             break;              
                        case GLFW_KEY_F:             break;              
                        case GLFW_KEY_G:             break;              
                        case GLFW_KEY_H:             break;              
                        case GLFW_KEY_I:             break;              
                        case GLFW_KEY_J:             break;              
                        case GLFW_KEY_K:             break;              
                        case GLFW_KEY_L:             break;              
                        case GLFW_KEY_M:             break;              
                        case GLFW_KEY_N:             break;              
                        case GLFW_KEY_O:             break;              
                        case GLFW_KEY_P:             break;              
                        case GLFW_KEY_Q:             break;              
                        case GLFW_KEY_R:             break;              
                        case GLFW_KEY_S:             event.key.code = Keyboard::S; break;              
                        case GLFW_KEY_T:             break;              
                        case GLFW_KEY_U:             break;              
                        case GLFW_KEY_V:             break;              
                        case GLFW_KEY_W:             event.key.code = Keyboard::W; break;              
                        case GLFW_KEY_X:             break;              
                        case GLFW_KEY_Y:             break;              
                        case GLFW_KEY_Z:             break;              
                        case GLFW_KEY_LEFT_BRACKET:  break;  /* [ */
                        case GLFW_KEY_BACKSLASH:     break; /* \ */
                        case GLFW_KEY_RIGHT_BRACKET: break; /* ] */
                        case GLFW_KEY_GRAVE_ACCENT:  break; /* ` */
                        case GLFW_KEY_WORLD_1:       break; /* non-US #1 */
                        case GLFW_KEY_WORLD_2:       break; /* non-US #2 */

                        /* Function keys */
                        case GLFW_KEY_ESCAPE:        event.key.code = Keyboard::Escape; break;       
                        case GLFW_KEY_ENTER:         break;        
                        case GLFW_KEY_TAB:           break;          
                        case GLFW_KEY_BACKSPACE:     break;    
                        case GLFW_KEY_INSERT:        break;       
                        case GLFW_KEY_DELETE:        break;       
                        case GLFW_KEY_RIGHT:         break;        
                        case GLFW_KEY_LEFT:          break;         
                        case GLFW_KEY_DOWN:          break;         
                        case GLFW_KEY_UP:            break;           
                        case GLFW_KEY_PAGE_UP:       break;      
                        case GLFW_KEY_PAGE_DOWN:     break;    
                        case GLFW_KEY_HOME:          break;         
                        case GLFW_KEY_END:           break;          
                        case GLFW_KEY_CAPS_LOCK:     break;    
                        case GLFW_KEY_SCROLL_LOCK:   break;  
                        case GLFW_KEY_NUM_LOCK:      break;     
                        case GLFW_KEY_PRINT_SCREEN:  break; 
                        case GLFW_KEY_PAUSE:         break;        
                        case GLFW_KEY_F1:            break;           
                        case GLFW_KEY_F2:            break;           
                        case GLFW_KEY_F3:            break;           
                        case GLFW_KEY_F4:            break;           
                        case GLFW_KEY_F5:            break;           
                        case GLFW_KEY_F6:            break;           
                        case GLFW_KEY_F7:            break;           
                        case GLFW_KEY_F8:            break;           
                        case GLFW_KEY_F9:            break;           
                        case GLFW_KEY_F10:           break;          
                        case GLFW_KEY_F11:           break;          
                        case GLFW_KEY_F12:           break;          
                        case GLFW_KEY_F13:           break;          
                        case GLFW_KEY_F14:           break;          
                        case GLFW_KEY_F15:           break;          
                        case GLFW_KEY_F16:           break;          
                        case GLFW_KEY_F17:           break;          
                        case GLFW_KEY_F18:           break;          
                        case GLFW_KEY_F19:           break;          
                        case GLFW_KEY_F20:           break;          
                        case GLFW_KEY_F21:           break;          
                        case GLFW_KEY_F22:           break;          
                        case GLFW_KEY_F23:           break;          
                        case GLFW_KEY_F24:           break;          
                        case GLFW_KEY_F25:           break;          
                        case GLFW_KEY_KP_0:          break;         
                        case GLFW_KEY_KP_1:          break;         
                        case GLFW_KEY_KP_2:          break;         
                        case GLFW_KEY_KP_3:          break;         
                        case GLFW_KEY_KP_4:          break;         
                        case GLFW_KEY_KP_5:          break;         
                        case GLFW_KEY_KP_6:          break;         
                        case GLFW_KEY_KP_7:          break;         
                        case GLFW_KEY_KP_8:          break;         
                        case GLFW_KEY_KP_9:          break;         
                        case GLFW_KEY_KP_DECIMAL:    break;   
                        case GLFW_KEY_KP_DIVIDE:     break;    
                        case GLFW_KEY_KP_MULTIPLY:   break;  
                        case GLFW_KEY_KP_SUBTRACT:   break;  
                        case GLFW_KEY_KP_ADD:        break;       
                        case GLFW_KEY_KP_ENTER:      break;     
                        case GLFW_KEY_KP_EQUAL:      break;     
                        case GLFW_KEY_LEFT_SHIFT:    break;   
                        case GLFW_KEY_LEFT_CONTROL:  break; 
                        case GLFW_KEY_LEFT_ALT:      break;     
                        case GLFW_KEY_LEFT_SUPER:    break;   
                        case GLFW_KEY_RIGHT_SHIFT:   break;  
                        case GLFW_KEY_RIGHT_CONTROL: break;
                        case GLFW_KEY_RIGHT_ALT:     break;    
                        case GLFW_KEY_RIGHT_SUPER:   break;  
                        case GLFW_KEY_MENU:          break;

                        default: event.key.code = Keyboard::Unknown;
                    }
                }

                base_window->pushEvent(event);
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


void BaseWindow::close() noexcept
{
    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_handle), GLFW_TRUE);
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


void* BaseWindow::getGLFWHandle() noexcept
{
    return m_handle;
}


bool BaseWindow::popEvent(Event& event) noexcept
{
    if(!m_events.empty())
    {
        event = m_events.front();
        m_events.pop();

        return true;
    }

    return false;
}


bool BaseWindow::isOpen() const noexcept
{
    return (m_handle != nullptr) && !glfwWindowShouldClose(static_cast<GLFWwindow*>(m_handle));
}


void BaseWindow::pushEvent(const Event& event) noexcept
{
    m_events.push(event);
}