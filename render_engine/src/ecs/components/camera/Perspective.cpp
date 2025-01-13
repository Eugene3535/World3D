#include <algorithm>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ecs/components/camera/Perspective.hpp"

Perspective::Perspective() noexcept:
    m_projection(glm::identity<glm::mat4>()),
    m_modelView(glm::identity<glm::mat4>()),
    m_eye(),
    m_delta(),
	m_pitch(0.0f),
	m_yaw(0.0f),
    m_uboMatrix(0),
    m_modelViewNeedUpdate(true)
{
    glGenBuffers(1, &m_uboMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrix);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatrix, 0, sizeof(glm::mat4));
}

glm::mat4 Perspective::getNatrix()
{
    return m_projection * m_modelView;
}


Perspective::~Perspective() noexcept
{
    glDeleteBuffers(1, &m_uboMatrix);
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

    // glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrix);
    // glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_projection * m_modelView));
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);
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


const glm::vec3& Perspective::getPosition() const noexcept
{
    return m_eye;
}


glm::vec3 Perspective::getLineOfSight() const noexcept
{
    const float tx = -sin(glm::radians(m_pitch));
    const float ty = tan(glm::radians(m_yaw));
    const float tz = -cos(glm::radians(m_pitch));

    return { tx, ty, tz };
}


void Perspective::recalculateModelViewMatrix() noexcept
{
    const float tx = m_eye.x - sin(glm::radians(m_pitch));
    const float ty = m_eye.y + tan(glm::radians(m_yaw));
    const float tz = m_eye.z - cos(glm::radians(m_pitch));

    m_modelView = glm::lookAt(m_eye, glm::vec3(tx, ty, tz), glm::vec3(0, 1, 0));
    m_modelViewNeedUpdate = false;
}