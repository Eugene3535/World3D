#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "scenes/platformer/entities/Entity.hpp"


class Player : public Entity
{
public:
	enum { stay, walk, duck, jump, climb, swim } state;
	bool onLadder, shoot, hit;

	struct
	{
		bool L, R, Up, Down, Space;
	} key;


	Player(const Animator& a, std::span<TileMap::Object> lev, int x, int y) noexcept:
		Entity(a, x, y)
	{
		setOptions(0, 100, "stay");
		state = stay; hit = false;
		objects = lev;
		key.R = key.L = key.Up = key.Down = key.Space = false;
	}

	void Keyboard() noexcept
	{
		if (key.L)
		{
			looksToTheRight = true;
			if (state != duck) dx = -150;
			if (state == stay) state = walk;
		}

		if (key.R)
		{
			looksToTheRight = false;
			if (state != duck) dx = 150;
			if (state == stay) state = walk;
		}

		if (key.Up)
		{
			if (onLadder) state = climb;
			if (state == stay || state == walk) { dy = -360; state = jump; anim.setAnimation("jump"); }
			if (state == climb) dy = -100;
			if (state == climb) if (key.L || key.R) state = stay;
		}

		if (key.Down)
		{
			if (state == stay || state == walk) { state = duck; dx = 0; }
			if (state == climb) dy = 100;
		}

		if (key.Space)
		{
			shoot = true;
		}

		/////////////////////если клавиша отпущена///////////////////////////
		if (!(key.R || key.L))
		{
			dx = 0;
			if (state == walk) state = stay;
		}

		if (!(key.Up || key.Down))
		{
			if (state == climb) dy = 0;
		}

		if (!key.Down)
		{
			if (state == duck) { state = stay; }
		}

		if (!key.Space)
		{
			shoot = false;
		}

		key.R = key.L = key.Up = key.Down = key.Space = false;
	}

	void Animation(float dt) noexcept
	{
		if (state == stay) anim.setAnimation("stay");
		if (state == walk) anim.setAnimation("walk");
		if (state == jump) anim.setAnimation("jump");
		if (state == duck) anim.setAnimation("duck");
		if (state == climb) { anim.setAnimation("climb"); anim.pause(); if (dy != 0) anim.play(); }

		if (shoot) 
		{
			anim.setAnimation("shoot");
			if (state == walk) 
				anim.setAnimation("shootAndWalk");
		}

		if (hit) 
		{
			timer += dt;
			if (timer > 1) { hit = false; timer = 0; }
			anim.setAnimation("hit");
		}

		if (looksToTheRight) setScale(-1, 1); else setScale(1, 1);

		anim.update(dt);
	}

	void update(float dt) noexcept
	{
		Keyboard();

		Animation(dt);

		if (state == climb) if (!onLadder) state = stay;
		if (state != climb) dy += 1000.0f * dt;
		onLadder = false;

		hitbox.left += dx * dt;
		Collision(0);

		hitbox.top += dy * dt;
		Collision(1);
	}

	void Collision(int num) noexcept
	{
		const auto& bounds = hitbox;

		for (int i = 0; i < objects.size(); i++)
			if (bounds.intersects(objects[i].bounds))
			{
				if (objects[i].name == "solid")
				{
					if (dy > 0 && num == 1) { hitbox.top  = objects[i].bounds.top - hitbox.height;  dy = 0;   state = stay; }
					if (dy < 0 && num == 1) { hitbox.top  = objects[i].bounds.top + objects[i].bounds.height;   dy = 0; }
					if (dx > 0 && num == 0) { hitbox.left = objects[i].bounds.left - hitbox.width; }
					if (dx < 0 && num == 0) { hitbox.left = objects[i].bounds.left + objects[i].bounds.width; }
				}

				if (objects[i].name == "ladder") { onLadder = true; if (state == climb) hitbox.left = objects[i].bounds.left - 10; }

				if (objects[i].name == "SlopeLeft")
				{
					const sf::FloatRect& r = objects[i].bounds;
					int y0 = (hitbox.left + hitbox.width * 0.5f - r.left) * r.height / r.width + r.top - hitbox.height;

					if (hitbox.top > y0)
					{
						if (hitbox.left + hitbox.width * 0.5f > r.left)
						{
							hitbox.top = y0; dy = 0; state = stay;
						}
					}
				}

				if (objects[i].name == "SlopeRight")
				{
					const sf::FloatRect& r = objects[i].bounds;
					int y0 = -(hitbox.left + hitbox.width * 0.5f - r.left) * r.height / r.width + r.top + r.height - hitbox.height;

					if (hitbox.top > y0)
						if (hitbox.left + hitbox.width * 0.5f < r.left + r.width)
						{
							hitbox.top = y0; 
							dy = 0; 
							state = stay;
						}
				}

			}
	}
};


#endif // !PLAYER_HPP
