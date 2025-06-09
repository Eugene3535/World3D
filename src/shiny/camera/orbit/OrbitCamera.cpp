#include <cglm/call/vec3.h>
#include <cglm/call/cam.h>

#include "camera/orbit/OrbitCamera.hpp"


OrbitCamera::OrbitCamera() noexcept:
    m_radius(0.f),
    m_minRadius(0.f),
    m_azimuth(0.f),
    m_polar(0.f),
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

    m_minRadius = m_radius * 0.3f;
    m_azimuth = 0;
    m_polar = GLM_PI_4f;
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::rotateAzimuth(float degrees) noexcept
{
    m_azimuth += glm_rad(degrees);

    // Keep azimuth angle within range <0...2PI> - it's not necessary, just to have it nicely output
    constexpr auto fullCircle = GLM_PIf * 2.f;
    m_azimuth = fmodf(m_azimuth, fullCircle);

    if (m_azimuth < 0.f) 
        m_azimuth = fullCircle + m_azimuth;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::rotatePolar(float degrees) noexcept
{
    m_polar += glm_rad(degrees);

    // Check if the angle hasn't exceeded quarter of a circle to prevent flip, add a bit of epsilon like 0.001 radians
    constexpr auto polarCap = GLM_PI_2f - 0.001f;
    m_polar = glm_clamp(m_polar, -polarCap, polarCap);
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::moveHorizontal(float distance) noexcept
{
    vec3 position;
    getEye(position);

    vec3 view; 
    getNormalizedViewVector(view);

    vec3 strafe;
    vec3 up = {0.f, 1.f, 0.f };
    glmc_vec3_crossn(view, up, strafe);

    m_target[0] += strafe[0] * distance;
    m_target[1] += strafe[1] * distance;
    m_target[2] += strafe[2] * distance;

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

    if (m_radius < m_minRadius) 
        m_radius = m_minRadius;

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
    const auto sineAzimuth   = sin(m_azimuth);
    const auto cosineAzimuth = cos(m_azimuth);
    const auto sinePolar     = sin(m_polar);
    const auto cosinePolar   = cos(m_polar);

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
