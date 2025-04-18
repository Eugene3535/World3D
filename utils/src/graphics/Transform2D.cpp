#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "graphics/Transform2D.hpp"


Transform2D::Transform2D() noexcept:
    m_matrix(glm::identity<glm::mat4>()),
    m_origin(0.0f, 0.0f),
    m_position(0.0f, 0.0f),
    m_scale(1.0f, 1.0f),
    m_rotation(0.0f),
    m_transformNeedUpdate(true)
{

}


Transform2D::~Transform2D() noexcept = default;


void Transform2D::setPosition(float x, float y) noexcept
{
    m_position.x = x;
    m_position.y = y;
    m_transformNeedUpdate = true;
}


void Transform2D::setPosition(const glm::vec2& position) noexcept
{
    setPosition(position.x, position.y);
}


void Transform2D::setRotation(float angle) noexcept
{
    m_rotation = static_cast<float>(fmod(angle, 360));

    if (m_rotation < 0)
        m_rotation += 360.f;

    m_transformNeedUpdate = true;
}


void Transform2D::setScale(float factorX, float factorY) noexcept
{
    m_scale.x = factorX;
    m_scale.y = factorY;
    m_transformNeedUpdate = true;
}


void Transform2D::setScale(const glm::vec2& factors) noexcept
{
    setScale(factors.x, factors.y);
}


void Transform2D::setScale(float factor) noexcept
{
    setScale(factor, factor);
}


void Transform2D::setOrigin(float x, float y) noexcept
{
    m_origin.x = x;
    m_origin.y = y;
    m_transformNeedUpdate = true;
}


void Transform2D::setOrigin(const glm::vec2& origin) noexcept
{
    setOrigin(origin.x, origin.y);
}


const glm::vec2& Transform2D::getPosition() const noexcept
{
    return m_position;
}


float Transform2D::getRotation() const noexcept
{
    return m_rotation;
}


const glm::vec2& Transform2D::getScale() const noexcept
{
    return m_scale;
}


const glm::vec2& Transform2D::getOrigin() const noexcept
{
    return m_origin;
}


void Transform2D::move(float offsetX, float offsetY) noexcept
{
    setPosition(m_position.x + offsetX, m_position.y + offsetY);
}


void Transform2D::move(const glm::vec2& offset) noexcept
{
    setPosition(m_position.x + offset.x, m_position.y + offset.y);
}


void Transform2D::rotate(float angle) noexcept
{
    setRotation(m_rotation + angle);
}


void Transform2D::scale(float factorX, float factorY) noexcept
{
    setScale(m_scale.x * factorX, m_scale.y * factorY);
}


void Transform2D::scale(const glm::vec2& factor) noexcept
{
    setScale(m_scale.x * factor.x, m_scale.y * factor.y);
}


const glm::mat4& Transform2D::getMatrix() const noexcept
{  
    // Recompute the matrix if needed
    if (m_transformNeedUpdate)
    {
        float angle  = glm::radians(-m_rotation);
        float cosine = cos(angle);
        float sine   = sin(angle);
        float sxc    = m_scale.x * cosine;
        float syc    = m_scale.y * cosine;
        float sxs    = m_scale.x * sine;
        float sys    = m_scale.y * sine;
        float tx     = -m_origin.x * sxc - m_origin.y * sys + m_position.x;
        float ty     =  m_origin.x * sxs - m_origin.y * syc + m_position.y;

        auto m = glm::value_ptr(m_matrix);

        m[0] = sxc;  m[4] = sys; m[8] = 0.f;  m[12] = tx;
        m[1] = -sxs; m[5] = syc; m[9] = 0.f;  m[13] = ty;
        m[2] = 0.f;  m[6] = 0.f; m[10] = 1.f; m[14] = 0.f;
        m[3] = 0.f;  m[7] = 0.f; m[11] = 0.f; m[15] = 1.f;

        m_transformNeedUpdate = false;
    }

    return m_matrix;
}


void Transform2D::loadIdentity() noexcept
{
    *this = Transform2D();
}