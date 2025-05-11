#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "scenes/platformer/entities/Entity.hpp"

#define GOOMBA_WALK "goomba_move"
#define GOOMBA_DEAD "goomba_dead"

class Goomba: public Entity
{
public:
Goomba(const Animator& a, int x, int y) noexcept:
		Entity(a, x, y)
	{
		setOptions(20.0f, 15, GOOMBA_WALK);
	}

	void update(float dt) noexcept
	{
		hitbox.position.x += dx * dt;
		timer += dt;

		if (timer > 3.0f)
		{
			dx *= -1;
			timer = 0;
		}

		if (Health < 1)
		{
			anim.setAnimation(GOOMBA_DEAD);
			dx = 0;
			timer_end += dt;

			if (timer_end > 2)
				isAlive = false;
		}

		setPosition(hitbox.position.x, hitbox.position.y);
		Entity::update(dt);
	}
};

#endif // !ENEMY_HPP
