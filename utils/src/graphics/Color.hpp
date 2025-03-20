#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

class Color
{
public:
    Color() noexcept;
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) noexcept;
    explicit Color(uint32_t color) noexcept;

    uint32_t toInteger()       const noexcept;
    void fromInteger(uint32_t color) noexcept;

    static const Color Black;      
    static const Color White;      
    static const Color Red;        
    static const Color Green;      
    static const Color Blue;       
    static const Color Yellow;     
    static const Color Magenta;    
    static const Color Cyan;       
    static const Color Transparent;

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

bool operator ==(const Color& left, const Color& right) noexcept;
bool operator !=(const Color& left, const Color& right) noexcept;

#endif // !COLOR_HPP