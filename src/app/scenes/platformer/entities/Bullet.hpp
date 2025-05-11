#ifndef BULLET_HPP
#define BULLET_HPP

#include "Entity.hpp"

#define BULLET_MOVE "bullet_move"
#define BULLET_EXPLODE "bullet_explode"

class Bullet: public Entity
{
public:
	Bullet(const Animator& a, std::span<const TileMap::Object> obj, int x, int y, bool dir) noexcept:
		Entity(a, x, y)
	{
		setOptions(400.0f, 10, BULLET_MOVE);

		if (dir) dx = -dx;
		objects = obj;
	}

	void update(float dt) noexcept
	{
		if(Health > 0)
		{
			hitbox.position.x += dx * dt;

			for (const auto& obj : objects)
				if (hitbox.findIntersection(obj.bounds))	
					Health = 0;	
		}

		if (Health < 1) 
		{
			anim.setAnimation(BULLET_EXPLODE); 
			anim.setLoop(false);
			dx = 0;

			if (!anim.isPlaying()) 
				isAlive = false;
		}

		uint32_t oX = anim.getWidth() >> 1;
		uint32_t oY = anim.getHeight() >> 1;

		setOrigin(oX, oY);
		setPosition(hitbox.position.x, hitbox.position.y);

		Entity::update(dt);
	}
};

#endif // !BULLET_HPP
