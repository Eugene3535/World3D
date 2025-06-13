#include <cglm/call/vec3.h>
#include <cglm/call/cam.h>

#include "camera/orbit/OrbitCamera.hpp"


OrbitCamera::OrbitCamera() noexcept:
    m_radius(0.f),
    m_azimuth(0.f),
    m_polar(45.f),
    m_modelViewNeedUpdate(true)
{
    glmc_mat4_identity(m_modelView);
    glmc_vec3_zero(m_target);
}


void OrbitCamera::setup(const vec3 minPoint, const vec3 maxPoint) noexcept
{
    m_target[0] = (minPoint[0] + maxPoint[0]) * 0.5f;
    m_target[1] = (minPoint[1] + maxPoint[1]) * 0.5f;
    m_target[2] = (minPoint[2] + maxPoint[2]) * 0.5f;

    vec3 halfSize;
    halfSize[0] = fabs(maxPoint[0] - minPoint[0]) * 0.5f;
    halfSize[1] = fabs(maxPoint[1] - minPoint[1]) * 0.5f;
    halfSize[2] = fabs(maxPoint[2] - minPoint[2]) * 0.5f;

    m_radius = glmc_vec3_norm(halfSize) * 1.5f;

    m_azimuth = 0;
    m_polar = GLM_PI_4f;
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::rotate(float dx, float dy) noexcept
{
    constexpr auto polarCap = 90.f - 0.001f;

    m_polar   = glm_clamp(m_polar + dy, -polarCap, polarCap);
    m_azimuth = fmodf(m_azimuth + dx, 360.f);

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::movePamoramic(float dx, float dy) noexcept
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


void OrbitCamera::zoom(float distance) noexcept
{
    m_radius += distance;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::getModelViewMatrix(mat4 m) noexcept
{
    if(m_modelViewNeedUpdate)
    {
        vec3 eye, view, center;
        vec3 worldUp = { 0.f, 1.f, 0.f };
        getEye(eye);
        getViewVector(view);

        glmc_vec3_normalize(view);
        glmc_vec3_add(eye, view, center);

        glmc_lookat(eye, center, worldUp, m_modelView);
        m_modelViewNeedUpdate = false;
    }

    glmc_mat4_copy(m_modelView, m);
}


void OrbitCamera::getEye(vec3 eye) const noexcept
{
    const float azimuth       = glm_rad(m_azimuth);
    const float polar         = glm_rad(m_polar);
    const float sineAzimuth   = sin(azimuth);
    const float cosineAzimuth = cos(azimuth);
    const float sinePolar     = sin(polar);
    const float cosinePolar   = cos(polar);

    eye[0] = m_target[0] + m_radius * cosinePolar * cosineAzimuth;
    eye[1] = m_target[1] + m_radius * sinePolar;
    eye[2] = m_target[2] + m_radius * cosinePolar * sineAzimuth;
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
    glmc_vec3_sub(target, eye, v);
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
    return m_radius;
}
