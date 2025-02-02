#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstdint>

#include "Export.hpp"


class OGL_API Scene
{
public:
	Scene() noexcept;
	virtual ~Scene() noexcept;

	virtual void draw() noexcept;

protected:

};

#endif // !SCENE_HPP
