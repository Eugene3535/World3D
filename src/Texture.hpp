#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstdint>

class Texture
{
public:
	uint32_t createFromImage(const class Image& image, int32_t wrap, int32_t filter) noexcept;
};

#endif // !TEXTURE_HPP