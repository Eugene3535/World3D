#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <vector>
#include <span>

#include <SFML/Graphics/Rect.hpp>

#include "graphics/common/Transform2D.hpp"
#include "scenes/platformer/animation/Animator.hpp"
#include "scenes/platformer/tilemap/TileMap.hpp"

class Entity: public Transform2D
{
public:
	Entity(const Animator& a, int x, int y) noexcept:
		Transform2D()
	{
		anim = a;
		hitbox.position.x = x;
		hitbox.position.y = y;
		looksToTheRight = false;
		Health = 0;

		isAlive = true;
		timer = 0;
		timer_end = 0;
		dx = 0;
		dy = 0;
	}

	virtual ~Entity() noexcept = default;

	virtual void update(float dt) noexcept { anim.update(dt); }

	void setOptions(float speed, int hp, const char* startAnim) noexcept
	{
		anim.setAnimation(startAnim);

		hitbox.size.x = anim.getWidth();
		hitbox.size.y = anim.getHeight();
		dx = speed;
		Health = hp;
	}

	template<class T>
	bool is() noexcept
	{
		return (dynamic_cast<T*>(this) != nullptr);
	}

	Animator anim;
	std::span<const TileMap::Object> objects;
	float timer, timer_end;
	sf::FloatRect hitbox;
	float dx, dy;
	int Health;
	bool isAlive, looksToTheRight;
};

#endif // ENTITY_HPP
