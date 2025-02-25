#include <algorithm>

#include "camera/Camera.hpp"


Camera::Camera() noexcept:
    m_projection(glm::identity<glm::mat4>()),
    m_modelView(glm::identity<glm::mat4>()),
    m_eye(),
    m_vectorFront(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_vectorUp(),
    m_vectorRight(),
    m_yaw(0.0f),
    m_pitch(0.0),
    m_fov(45.0f),
    m_aspect(0.0f),
    m_drawDistance(10.0f)
{
    recalculateModelViewMatrix();
}


void Camera::updateProjectionMatrix(float aspect) noexcept
{
    m_aspect = aspect;
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, 0.1f, m_drawDistance);
}


glm::mat4 Camera::getModelViewProjectionMatrix() const noexcept
{
    return m_projection * m_modelView;
}


void Camera::setDrawDistance(float distance) noexcept
{
    m_drawDistance = fabs(distance);
    updateProjectionMatrix(m_aspect);
}


void Camera::setPosition(float x, float y, float z) noexcept
{
    m_eye = { x, y, z };
}


void Camera::setPosition(const glm::vec3& position) noexcept
{
    m_eye = position;
}


void Camera::processKeyboard(Camera::Direction direction, float velocity) noexcept
{
    switch (direction)
    {
        case Camera::Forward:  m_eye += m_vectorFront * velocity; break;
        case Camera::Backward: m_eye -= m_vectorFront * velocity; break;
        case Camera::Left:     m_eye -= m_vectorRight * velocity; break;
        case Camera::Right:    m_eye += m_vectorRight * velocity; break;
        case Camera::Up:       m_eye.y += velocity; break;
        case Camera::Down:     m_eye.y -= velocity; break;

        default: break;
    }    
}


void Camera::processMouseMovement(float xoffset, float yoffset) noexcept
{
    m_yaw -= xoffset;
    m_pitch += yoffset;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    recalculateModelViewMatrix();
}


void Camera::processMouseScroll(float delta) noexcept
{
    m_fov -= delta;
    m_fov = std::clamp(m_fov, 1.0f, 45.0f);
}


const glm::vec3& Camera::getPosition() const noexcept
{
    return m_eye;
}


void Camera::recalculateModelViewMatrix() noexcept
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_vectorFront = glm::normalize(front);
    m_vectorRight = glm::normalize(glm::cross(m_vectorFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_vectorUp = glm::normalize(glm::cross(m_vectorRight, m_vectorFront));

    m_modelView = glm::lookAt(m_eye, m_eye + m_vectorFront, m_vectorUp);
}