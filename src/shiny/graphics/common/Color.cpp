#include "graphics/common/Color.hpp"


const Color Color::Black(0, 0, 0);
const Color Color::White(255, 255, 255);
const Color Color::Red(255, 0, 0);
const Color Color::Green(0, 255, 0);
const Color Color::Blue(0, 0, 255);
const Color Color::Yellow(255, 255, 0);
const Color Color::Magenta(255, 0, 255);
const Color Color::Cyan(0, 255, 255);
const Color Color::Transparent(0, 0, 0, 0);


Color::Color() noexcept: 
    r(0), g(0), b(0), a(255)
{

}


Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) noexcept:
    r(red), g(green), b(blue), a(alpha)
{

}


Color::Color(uint32_t color) noexcept :
    r(static_cast<uint8_t>((color & 0xff000000) >> 24)),
    g(static_cast<uint8_t>((color & 0x00ff0000) >> 16)),
    b((color & 0x0000ff00) >> 8 ),
    a((color & 0x000000ff) >> 0 )
{

}


uint32_t Color::toInteger() const noexcept
{
    return static_cast<uint32_t>((r << 24) | (g << 16) | (b << 8) | a);
}


void Color::fromInteger(uint32_t color) noexcept
{
    r = static_cast<uint8_t>((color & 0xff000000) >> 24);
    g = static_cast<uint8_t>((color & 0x00ff0000) >> 16);
    b = (color & 0x0000ff00) >> 8;
    a = (color & 0x000000ff) >> 0;
}


bool operator ==(const Color& left, const Color& right) noexcept
{
    return (left.r == right.r) &&
           (left.g == right.g) &&
           (left.b == right.b) &&
           (left.a == right.a);
}


bool operator !=(const Color& left, const Color& right) noexcept
{
    return !(left == right);
}