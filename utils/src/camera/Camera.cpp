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
    m_velocity(0.0f),
    m_mouseSensitivity(0.125f)
{
    recalculateModelViewMatrix();
}


glm::mat4 Camera::getModelViewProjectionMatrix() const noexcept
{
    return m_projection * m_modelView;
}


void Camera::processKeyboard(Camera::Direction direction, float deltaTime) noexcept
{
    float velocity = m_velocity * deltaTime;

    switch (direction)
    {
        case Camera::FORWARD:  m_eye += m_vectorFront * velocity; break;
        case Camera::BACKWARD: m_eye -= m_vectorFront * velocity; break;
        case Camera::LEFT:     m_eye -= m_vectorRight * velocity; break;
        case Camera::RIGHT:    m_eye += m_vectorRight * velocity; break;

        default: break;
    }    
}


void Camera::processMouseMovement(float xoffset, float yoffset) noexcept
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    recalculateModelViewMatrix();
}


void Camera::processMouseScroll(float delta) noexcept
{
    m_fov -= delta;
    m_fov = std::clamp(m_fov, 1.0f, 45.0f);
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