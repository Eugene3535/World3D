#ifndef SPRITE_2D_HPP
#define SPRITE_2D_HPP

#include <cstdint>

struct Sprite2D
{
	uint32_t texture = 0U;
	uint32_t frame   = 0U;
	uint32_t width   = 0U;
	uint32_t height  = 0U;
};

#endif // !SPRITE2D_HPP