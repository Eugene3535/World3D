#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <string>
#include <unordered_map>
#include <span>

#include "scenes/platformer/sprites/Sprite2D.hpp"

class Animator
{
public:
	struct Status
	{
		bool isPlaying    = false;
		bool isLooped     = false;
		bool isOver       = false;
		bool isReversed   = false;
	};

public:
	Animator() noexcept;

	bool addAnimation(const std::string& name, std::span<const Sprite2D> animation) noexcept;
	void setAnimation(const std::string& name) noexcept;

	void play() noexcept;
	void stop() noexcept;
	void update(float dt) noexcept;
	void draw() const noexcept;

	void setLoop(bool loop) noexcept;
	void setRate(float rate) noexcept;

	bool isLooped() const noexcept;
	bool isOver() const noexcept;

private:
	std::unordered_map<std::string, std::span<const Sprite2D>> m_animations;
	std::span<const Sprite2D> m_current;

	float m_timer;
	float m_rate;
	size_t m_frame;

	bool m_isPlaying;
	bool m_isLooped;
};

#endif // !ANIMATION_HPP