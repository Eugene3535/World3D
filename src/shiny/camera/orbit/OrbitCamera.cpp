#include <cglm/call/vec3.h>
#include <cglm/call/cam.h>

#include "camera/orbit/OrbitCamera.hpp"


OrbitCamera::OrbitCamera() noexcept:
    m_radius(0.f),
    m_azimuth(0.f),
    m_polar(30.f),
    m_modelViewNeedUpdate(true)
{
    glmc_mat4_identity(m_modelView);
    glmc_vec3_zero(m_eye);
    glmc_vec3_zero(m_target);
}


void OrbitCamera::focusOn(vec3 target) noexcept
{
    glmc_vec3_copy(target, m_target);
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::setPosition(vec3 eye) noexcept
{
    glmc_vec3_copy(eye, m_eye);
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::rotateAroundTarget(float dx, float dy) noexcept
{
    constexpr auto polarCap = 90.f - 0.001f;

    m_polar   = glm_clamp(m_polar + dy, -polarCap, polarCap);
    m_azimuth = fmod(m_azimuth + dx, 360.f);

    if(m_azimuth < 0.f)
        m_azimuth += 360.f;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::movePanoramic(float dx, float dy) noexcept
{
    vec3 front, right, up;
    vec3 worldUp = { 0.f, 1.f, 0.f };

    getViewVector(front);
    glmc_vec3_crossn(front, worldUp, right);
    glmc_vec3_crossn(right, front, up);

    glmc_vec3_muladds(right, dx, m_target);
    glmc_vec3_muladds(up, dy, m_target);

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::move(Direction direction, float distance) noexcept
{
    vec3 front, right, up;
    vec3 worldUp = { 0.f, 1.f, 0.f };

    getViewVector(front);
    glmc_vec3_crossn(front, worldUp, right);
    glmc_vec3_crossn(right, front, up);

    switch (direction)
    {
        case OrbitCamera::Forward:
            if(m_radius > 10.f)
                glmc_vec3_muladds(front, distance, m_eye);
        break;

        case OrbitCamera::Backward:
            glmc_vec3_mulsubs(front, distance, m_eye);
        break;

        case OrbitCamera::Left:
            glmc_vec3_mulsubs(right, distance, m_eye);
        break;

        case OrbitCamera::Right:
            glmc_vec3_muladds(right, distance, m_eye);
        break;

        case OrbitCamera::Up:
            glmc_vec3_muladds(up, distance, m_eye);
        break;

        case OrbitCamera::Down:
            glmc_vec3_mulsubs(up, distance, m_eye);
        break;

        default:
            break;
    }

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::getModelViewMatrix(mat4 m) noexcept
{
    if(m_modelViewNeedUpdate)
    {
        vec3 eye, center;
        vec3 worldUp = { 0.f, 1.f, 0.f };
        getEye(eye);
        glmc_lookat(m_eye, m_target, worldUp, m_modelView);

        m_modelViewNeedUpdate = false;
    }

    glmc_mat4_copy(m_modelView, m);
}


void OrbitCamera::getEye(vec3 eye) const noexcept
{
    if(m_modelViewNeedUpdate)
    {
        const float azimuth       = glm_rad(m_azimuth);
        const float polar         = glm_rad(m_polar);
        const float sineAzimuth   = sin(azimuth);
        const float cosineAzimuth = cos(azimuth);
        const float sinePolar     = sin(polar);
        const float cosinePolar   = cos(polar);

        m_radius = getRadius();

        m_eye[0] = m_target[0] + m_radius * cosinePolar * cosineAzimuth;
        m_eye[1] = m_target[1] + m_radius * sinePolar;
        m_eye[2] = m_target[2] + m_radius * cosinePolar * sineAzimuth;
    }

    glm_vec3_copy(m_eye, eye);
}


void OrbitCamera::getTarget(vec3 target) const noexcept
{
    glmc_vec3_copy(m_target, target);
}


void OrbitCamera::getViewVector(vec3 v) const noexcept
{
    vec3 target, eye;
    getTarget(target);
    getEye(eye);
    glmc_vec3_sub(eye, target, v);
}


float OrbitCamera::getAzimuth() const noexcept
{
    return m_azimuth;
}


float OrbitCamera::getPolar() const noexcept
{
    return m_polar;
}


float OrbitCamera::getRadius() const noexcept
{
    if(m_modelViewNeedUpdate)
    {
        vec3 distance;
        distance[0] = fabs(m_target[0] - m_eye[0]);
        distance[1] = fabs(m_target[1] - m_eye[1]);
        distance[2] = fabs(m_target[2] - m_eye[2]);
        m_radius = glmc_vec3_norm(distance);
    }

    return m_radius;
}
