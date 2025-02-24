#include <algorithm>
#include <cstdio>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera/perspective/Perspective.hpp"


Perspective::Perspective() noexcept:
    m_projection(glm::identity<glm::mat4>()),
    m_modelView(glm::identity<glm::mat4>()),
    m_eye(),
    m_delta(),
	m_pitch(0.0f),
	m_yaw(0.0f),
    m_modelViewNeedUpdate(true)
{
    
}


void Perspective::setupProjectionMatrix(float fovy, float aspect, float zNear, float zFar) noexcept
{
    m_projection = glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
}


void Perspective::apply(float dt) noexcept
{
    m_eye.x += m_delta.x * dt;
    m_eye.y += m_delta.y * dt;
    m_eye.z += m_delta.z * dt;

    m_delta = { 0.0f, 0.0f, 0.0f };

    if(m_modelViewNeedUpdate)
        recalculateModelViewMatrix();
}


void Perspective::setPosition(float x, float y, float z) noexcept
{
    m_eye = { x, y, z };
    m_modelViewNeedUpdate = true;
}


void Perspective::setPosition(const glm::vec3& position) noexcept
{
    m_eye = position;
    m_modelViewNeedUpdate = true;
}


void Perspective::rotateX(float pitch) noexcept
{
    m_pitch += pitch;
    m_modelViewNeedUpdate = true;
}


void Perspective::rotateY(float yaw) noexcept
{
    m_yaw += yaw;
    m_yaw = std::clamp(m_yaw, -89.0f, 89.0f);
    m_modelViewNeedUpdate = true;
}


void Perspective::moveForward(float velocity) noexcept
{
    m_delta.x = -sin(glm::radians(m_pitch)) * velocity;
    m_delta.z = -cos(glm::radians(m_pitch)) * velocity;
    m_modelViewNeedUpdate = true;
}


void Perspective::moveBackward(float velocity) noexcept
{
    m_delta.x = sin(glm::radians(m_pitch)) * velocity;
    m_delta.z = cos(glm::radians(m_pitch)) * velocity;
    m_modelViewNeedUpdate = true;
}


void Perspective::moveLeft(float velocity) noexcept
{
    m_delta.x = sin(glm::radians(m_pitch) - glm::half_pi<float>()) * velocity;
    m_delta.z = cos(glm::radians(m_pitch) - glm::half_pi<float>()) * velocity;
    m_modelViewNeedUpdate = true;
}


void Perspective::moveRight(float velocity) noexcept
{
    m_delta.x = sin(glm::radians(m_pitch) + glm::half_pi<float>()) * velocity;
    m_delta.z = cos(glm::radians(m_pitch) + glm::half_pi<float>()) * velocity;
    m_modelViewNeedUpdate = true;
}


void Perspective::moveUp(float velocity) noexcept
{
    m_delta.y = velocity;
    m_modelViewNeedUpdate = true;
}


void Perspective::moveDown(float velocity) noexcept
{
    m_delta.y = -velocity;
    m_modelViewNeedUpdate = true;
}


void Perspective::moveToPoint(float velocity) noexcept
{
    m_delta = getLineOfSight() * velocity;
    m_modelViewNeedUpdate = true;
}


void Perspective::revertToOrigin(float height) noexcept
{
    m_eye = { 0, height, 0 };
    m_yaw = -45;
    m_pitch = -135;

    m_modelViewNeedUpdate = true;
}


const glm::vec3& Perspective::getPosition() const noexcept
{
    return m_eye;
}


glm::vec3 Perspective::getLineOfSight() const noexcept
{
    const float tx = -sin(glm::radians(m_pitch));
    const float ty =  tan(glm::radians(m_yaw));
    const float tz = -cos(glm::radians(m_pitch));

    return { tx, ty, tz };
}


glm::mat4 Perspective::getModelViewProjectionMatrix() const noexcept
{
    return m_projection * m_modelView;
}


void Perspective::recalculateModelViewMatrix() noexcept
{
    const float tx = m_eye.x - sin(glm::radians(m_pitch));
    const float ty = m_eye.y + tan(glm::radians(m_yaw));
    const float tz = m_eye.z - cos(glm::radians(m_pitch));

    m_modelView = glm::lookAt(m_eye, glm::vec3(tx, ty, tz), glm::vec3(0, 1, 0));
    m_modelViewNeedUpdate = false;
}