#include <cglm/cam.h>

#include "camera/perspective/PerspectiveCamera.hpp"


PerspectiveCamera::PerspectiveCamera() noexcept:
    m_eye(),
    m_vectorUp(),
    m_vectorRight(),
    m_yaw(0.0f),
    m_pitch(0.0),
    m_fov(45.0f),
    m_aspect(0.0f),
    m_drawDistance(10.0f),
    m_modelViewNeedUpdate(true)
{
    glm_mat4_identity(m_projection);
    glm_mat4_identity(m_modelView);
    glm_vec3_zero(m_vectorFront);
    m_vectorFront[2] = -1;
}


void PerspectiveCamera::updateProjectionMatrix(float aspect) noexcept
{
    m_aspect = aspect;
    glm_perspective(glm_rad(45), m_aspect, 0.1f, m_drawDistance, m_projection);
}


void PerspectiveCamera::getModelViewProjectionMatrix(mat4 mvp) noexcept
{
    if (m_modelViewNeedUpdate)
    {
        recalculateModelViewMatrix();
        m_modelViewNeedUpdate = false;
    }

    glm_mat4_mul(m_projection, m_modelView, mvp);
}


void PerspectiveCamera::setDrawDistance(float distance) noexcept
{
    m_drawDistance = fabs(distance);
    updateProjectionMatrix(m_aspect);
}


void PerspectiveCamera::setPosition(float x, float y, float z) noexcept
{
    m_eye[0] = x;
    m_eye[1] = y;
    m_eye[2] = z;
    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::setPosition(vec3 position) noexcept
{
    glm_vec3_copy(position, m_eye);
    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::processKeyboard(PerspectiveCamera::Direction direction, float velocity) noexcept
{
    auto add_offset = [](vec3 dir, float vel, vec3 eye) -> void
    {
        eye[0] += dir[0] * vel;
        eye[1] += dir[1] * vel;
        eye[2] += dir[2] * vel;
    };

    auto sub_offset = [](vec3 dir, float vel, vec3 eye) -> void
    {
        eye[0] -= dir[0] * vel;
        eye[1] -= dir[1] * vel;
        eye[2] -= dir[2] * vel;
    };

    switch (direction)
    {
        case PerspectiveCamera::Forward:  add_offset(m_vectorFront, velocity, m_eye); break;
        case PerspectiveCamera::Backward: sub_offset(m_vectorFront, velocity, m_eye); break;
        case PerspectiveCamera::Left:     sub_offset(m_vectorRight, velocity, m_eye); break;
        case PerspectiveCamera::Right:    add_offset(m_vectorRight, velocity, m_eye); break;
        case PerspectiveCamera::Up:       m_eye[1] += velocity; break;
        case PerspectiveCamera::Down:     m_eye[1] -= velocity; break;

        default: break;
    }

    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::processMouseMovement(float xoffset, float yoffset) noexcept
{
    m_yaw -= xoffset;
    m_pitch += yoffset;
    m_pitch = glm_clamp(m_pitch, -89.0f, 89.0f);
    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::processMouseScroll(float delta) noexcept
{
    m_fov -= delta;
    m_fov = glm_clamp(m_fov, 1.0f, 45.0f);
    m_modelViewNeedUpdate = true;
}


void PerspectiveCamera::getPosition(vec3 position) const noexcept
{
    position[0] = m_eye[0];
    position[1] = m_eye[1];
    position[2] = m_eye[2];
}


void PerspectiveCamera::recalculateModelViewMatrix() noexcept
{
    vec3 front = 
    {
        cos(glm_rad(m_yaw)) * cos(glm_rad(m_pitch)),
        sin(glm_rad(m_pitch)),
        sin(glm_rad(m_yaw)) * cos(glm_rad(m_pitch))
    };


    glm_vec3_normalize_to(front, m_vectorFront);
    glm_vec3_crossn(m_vectorFront, (vec3){ 0.0f, 1.0f, 0.0f }, m_vectorRight);
    glm_vec3_crossn(m_vectorRight, m_vectorFront, m_vectorUp);

    vec3 center;
    glm_vec3_add(m_eye, m_vectorFront, center);

    glm_lookat(m_eye, center, m_vectorUp, m_modelView);
}