#ifndef APP_DATA_HPP
#define APP_DATA_HPP

#include "opengl/holder/GlResourceHolder.hpp"
#include "camera/orthogonal/Orthogonal.hpp"
#include "camera/perspective/Perspective.hpp"

struct AppData
{
	GlResourceHolder resourceHolder;

	struct
	{
		Orthogonal orthogonal;
		Perspective perspective;
	} camera;
};

#endif // !APP_DATA_HPP
