#ifndef EVENT_HPP
#define EVENT_HPP

#include <cstdint>


class Keyboard
{
public:
    enum Key
    {
        Unknown = -1,
        A = 0, 
        B, 
        C, 
        D, 
        E, 
        F, 
        G, 
        H, 
        I, 
        J, 
        K, 
        L, 
        M, 
        N, 
        O, 
        P, 
        Q, 
        R, 
        S, 
        T, 
        U, 
        V, 
        W, 
        X, 
        Y, 
        Z,  
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        Escape,
        LControl,
        LShift,
        LAlt,
        LSystem, 
        RControl,
        RShift,  
        RAlt,  
        RSystem,
        Menu,
        LBracket,
        RBracket,
        Semicolon,
        Comma,
        Period,
        Apostrophe,   
        Slash,        
        Backslash,    
        Grave,        
        Equal,        
        Hyphen,       
        Space,        
        Enter,      
        Backspace,    
        Tab,          
        PageUp,       
        PageDown,     
        End,          
        Home,         
        Insert,       
        Delete,       
        Add,          
        Subtract,
        Multiply,     
        Divide,    
        Left,    
        Right,    
        Up,    
        Down,   
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        Pause,

        KeyCount
    };
};


class Mouse
{
public:
    enum Button
    {
        Left,
        Right,
        Middle,

        ButtonCount
    };

    enum Wheel
    {
        VerticalWheel,
        HorizontalWheel
    };
};


class Event
{
public:
    struct SizeEvent
    {
        uint32_t width;
        uint32_t height;
    };

    struct KeyEvent
    {
        Keyboard::Key code;
        bool          alt;
        bool          control;   
        bool          shift;  
        bool          system;  
    };

    struct MouseMoveEvent
    {
        int32_t x;
        int32_t y;
    };

    struct MouseButtonEvent
    {
        Mouse::Button button;
        int32_t           x;
        int32_t           y;
    };

    struct MouseWheelEvent
    {
        int32_t delta;
        int32_t x;
        int32_t y;
    };

    struct MouseWheelScrollEvent
    {
        Mouse::Wheel wheel;
        float        delta;
        int32_t          x;
        int32_t          y;
    };

    enum EventType
    {
        Closed, 
        Resized, 
        LostFocus,
        GainedFocus, 
        KeyPressed, 
        KeyReleased,
        MouseWheelMoved,
        MouseWheelScrolled,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved, 
        MouseEntered,
        MouseLeft,      

        Count                  
    };

    EventType type;

    union
    {
        SizeEvent             size;
        KeyEvent              key;    
        MouseMoveEvent        mouseMove;
        MouseButtonEvent      mouseButton;
        MouseWheelEvent       mouseWheel;
        MouseWheelScrollEvent mouseWheelScroll;
    };
};

#endif // !EVENT_HPP