#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstdint>

#include "Export.hpp"


class RE_API Scene
{
public:
	Scene(void* handle) noexcept;
	virtual ~Scene() noexcept;

	virtual void draw() noexcept;

	void drawHeightmap(uint32_t numStrips, uint32_t numTrisPerStrip) noexcept;

protected:
	void* m_handle;
};

#endif // !SCENE_HPP
