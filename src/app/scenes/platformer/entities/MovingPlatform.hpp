#ifndef MOVING_PLATFORM_HPP
#define MOVING_PLATFORM_HPP

#include "Entity.hpp"

#define PLATFORM_MOVE "platform_move"

class MovingPlatform: public Entity
{
public:
	MovingPlatform(const Animator& a, int x, int y) noexcept:
		Entity(a, x, y)
	{
		setOptions(60.0f, 0, PLATFORM_MOVE);
	}

	void update(float dt) noexcept
	{
		hitbox.left += dx * dt;
		timer += dt;

		if (timer > 4.0f)
		{
			dx *= -1;
			timer = 0;
		}

		setPosition(hitbox.left, hitbox.top);
		Entity::update(dt);
	}
};

#endif // !MOVING_PLATFORM_HPP
