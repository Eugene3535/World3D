#include <algorithm>

#include "camera/perspective/PerspectiveCamera.hpp"


PerspectiveCamera::PerspectiveCamera() noexcept:
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
    m_drawDistance(10.0f),
    m_modelViewNeedUpdate(true)
{
    
}


void PerspectiveCamera::updateProjectionMatrix(float aspect) noexcept
{
    m_aspect = aspect;
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, 0.1f, m_drawDistance);
}


glm::mat4 PerspectiveCamera::getModelViewProjectionMatrix() noexcept
{
    if (m_modelViewNeedUpdate)
    {
        recalculateModelViewMatrix();
        m_modelViewNeedUpdate = false;
    }

    return m_projection * m_modelView;
}


void PerspectiveCamera::setDrawDistance(float distance) noexcept
{
    m_drawDistance = fabs(distance);
    updateProjectionMatrix(m_aspect);
}


void PerspectiveCamera::setPosition(float x, float y, float z) noexcept
{
    m_eye = { x, y, z };
    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::setPosition(const glm::vec3& position) noexcept
{
    m_eye = position;
    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::processKeyboard(PerspectiveCamera::Direction direction, float velocity) noexcept
{
    switch (direction)
    {
        case PerspectiveCamera::Forward:  m_eye += m_vectorFront * velocity; break;
        case PerspectiveCamera::Backward: m_eye -= m_vectorFront * velocity; break;
        case PerspectiveCamera::Left:     m_eye -= m_vectorRight * velocity; break;
        case PerspectiveCamera::Right:    m_eye += m_vectorRight * velocity; break;
        case PerspectiveCamera::Up:       m_eye.y += velocity; break;
        case PerspectiveCamera::Down:     m_eye.y -= velocity; break;

        default: break;
    }

    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::processMouseMovement(float xoffset, float yoffset) noexcept
{
    m_yaw -= xoffset;
    m_pitch += yoffset;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::processMouseScroll(float delta) noexcept
{
    m_fov -= delta;
    m_fov = std::clamp(m_fov, 1.0f, 45.0f);
    m_modelViewNeedUpdate = true;
}


const glm::vec3& PerspectiveCamera::getPosition() const noexcept
{
    return m_eye;
}


void PerspectiveCamera::recalculateModelViewMatrix() noexcept
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