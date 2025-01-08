#ifndef SCENE_HPP
#define SCENE_HPP

#include "Export.hpp"
#include "glad/glad.h"

class RL_API Scene
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
