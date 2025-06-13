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


void OrbitCamera::rotateAzimuth(float degrees) noexcept
{
    m_azimuth = fmodf(m_azimuth + degrees, 360.f);

    if (m_azimuth < 0.f) 
        m_azimuth = 360.f;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::rotatePolar(float degrees) noexcept
{
    m_polar += degrees;
    constexpr auto polarCap = 90.f - 0.001f;
    m_polar = glm_clamp(m_polar, -polarCap, polarCap);
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::moveHorizontal(float distance) noexcept
{
    vec3 position, view, strafe;
    vec3 worldUp = { 0.f, 1.f, 0.f };

    getEye(position);
    getNormalizedViewVector(view);

    glmc_vec3_crossn(view, worldUp, strafe);
    glmc_vec3_muladds(strafe, distance, m_target);

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::moveVertical(float distance) noexcept
{
    vec3 target, eye, front, right, up;
    vec3 worldUp = { 0.f, 1.f, 0.f };

    getTarget(target);
    getEye(eye);

    glmc_vec3_sub(target, eye, front);
    glmc_vec3_crossn(front, worldUp, right);
    glmc_vec3_crossn(right, front, up);
    glmc_vec3_muladds(up, distance, m_target);

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
        getNormalizedViewVector(view);
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


void OrbitCamera::getNormalizedViewVector(vec3 v) const noexcept
{
    vec3 target, eye;
    getTarget(target);
    getEye(eye);
    glmc_vec3_sub(target, eye, v);
    glmc_vec3_normalize(v);
}


float OrbitCamera::getAzimuthAngle() const noexcept
{
    return m_azimuth;
}


float OrbitCamera::getPolarAngle() const noexcept
{
    return m_polar;
}


float OrbitCamera::getRadius() const noexcept
{
    return m_radius;
}
