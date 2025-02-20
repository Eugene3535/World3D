#include <cmath>

#include <glad/glad.h>

#include "scenes/platformer/animation/Animator.hpp"


Animator::Animator() noexcept:
    m_timer(0.0f),
    m_rate(0.0f),
    m_isPlaying(false),
    m_isLooped(false),
    m_frame(0)
{

}


bool Animator::addAnimation(const std::string& name, std::span<const Sprite2D> animation) noexcept
{
    if(auto it = m_animations.try_emplace(name, animation); it.second)
    {
        m_current = it.first->second;

        return true;
    }

    return false;
}


void Animator::setAnimation(const std::string& name) noexcept
{
    if(auto found = m_animations.find(name); found != m_animations.end())
    {
        if(m_current.data() != found->second.data())
        {
            m_current = found->second; 
        }
    }
}


void Animator::play() noexcept
{
    if( ! m_current.empty() && ! isOver() )
        m_isPlaying = true;
}


void Animator::stop() noexcept
{
    m_isPlaying = false;
}


void Animator::update(float dt) noexcept
{
    if(m_isPlaying)
    {
        m_timer += dt * m_rate;

        if(!isOver())
        {
            m_frame = static_cast<size_t>(m_timer);
        }
        else
        {
            if(m_isLooped)
            {
                m_timer = 0;
            }
            else
            {
                m_isPlaying = false;
            }
        }
    }
}


void Animator::draw() const noexcept
{
	if(!m_current.empty())
	{
        const auto& sprite = m_current[m_frame];

		glBindTexture(GL_TEXTURE_2D, sprite.texture);
		glDrawArrays(GL_TRIANGLE_FAN, sprite.frame, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


void Animator::setLoop(bool loop) noexcept
{
    m_isLooped = loop;
}


void Animator::setRate(float rate) noexcept
{
    m_rate = fabs(rate);
}


float Animator::getRate() const noexcept
{
    return m_rate;
}


bool Animator::isLooped() const noexcept
{
    return m_isLooped;
}


bool Animator::isOver() const noexcept
{
//  In C and C++, we all know that converting a floating point value into an integer performs a truncation. 
//  That means, a fix towards zero, both for static_cast and for C style casts.
    float limit = static_cast<float>(m_current.size()) - 0.1f;

    return m_timer > limit;
}


int32_t Animator::getWidth() const noexcept
{
    if(!m_current.empty())
        return m_current[m_frame].width;

    return 0;
}


int32_t Animator::getHeight() const noexcept
{
    if(!m_current.empty())
        return m_current[m_frame].height;

    return 0;
}