#include <cglm/cam.h>

#include "camera/orbit/OrbitCamera.hpp"


OrbitCamera::OrbitCamera() noexcept:
    m_radius(0.f),
    m_minRadius(0.f),
    m_azimuth(0.f),
    m_polar(GLM_PI_4f),
    m_aspect(0.f),
    m_drawDistance(3.f),
    m_modelViewNeedUpdate(true)
{
    glm_vec3_zero(m_center);
    glm_mat4_identity(m_projection);
    glm_mat4_identity(m_modelView);
}


void OrbitCamera::setup(const vec3 minPoint, const vec3 maxPoint) noexcept
{
    m_center[0] = (minPoint[0] + maxPoint[0]) * 0.5f;
    m_center[1] = (minPoint[1] + maxPoint[1]) * 0.5f;
    m_center[2] = (minPoint[2] + maxPoint[2]) * 0.5f;

    vec3 halfSize = 
    {
        (maxPoint[0] - minPoint[0]) * 0.5f,
        (maxPoint[1] - minPoint[1]) * 0.5f,
        (maxPoint[2] - minPoint[2]) * 0.5f
    };

    m_radius = glm_vec3_norm(halfSize) * 1.5f;

    m_minRadius = m_radius * 0.3f;
    m_azimuth = 0;
    m_polar = GLM_PI_4f;
    m_drawDistance = m_radius * 2;
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::updateProjectionMatrix(float aspect) noexcept
{
    m_aspect = aspect;
    glm_perspective(glm_rad(45), m_aspect, 0.1f, m_drawDistance, m_projection);
}


void OrbitCamera::setDrawDistance(float distance) noexcept
{
    m_drawDistance = fabs(distance);
    updateProjectionMatrix(m_aspect);
}


void OrbitCamera::rotateAzimuth(float radians) noexcept
{
    m_azimuth += radians;

    // Keep azimuth angle within range <0..2PI) - it's not necessary, just to have it nicely output
    const auto fullCircle = GLM_PIf * 2.f;
    m_azimuth = fmodf(m_azimuth, fullCircle);

    if (m_azimuth < 0.f) 
        m_azimuth = fullCircle + m_azimuth;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::rotatePolar(float radians) noexcept
{
    m_polar += radians;

    // Check if the angle hasn't exceeded quarter of a circle to prevent flip, add a bit of epsilon like 0.001 radians
    const auto polarCap = GLM_PI_2f - 0.001f;
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
    glm_vec3_crossn(view, (vec3){ 0.f, 1.f, 0.f }, strafe);

    m_center[0] += strafe[0] * distance;
    m_center[1] += strafe[1] * distance;
    m_center[2] += strafe[2] * distance;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::moveVertical(float distance) noexcept
{
    vec3 view, eye, front, right, up;

    getViewPoint(view);
    getEye(eye);

    glm_vec3_sub(view, eye, front);
    glm_vec3_crossn(front, (vec3){ 0.0f, 1.0f, 0.0f }, right);
    glm_vec3_crossn(right, front, up);

    m_center[0] += up[0] * distance;
    m_center[1] += up[1] * distance;
    m_center[2] += up[2] * distance;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::zoom(float distance) noexcept
{
    m_radius += distance;

    if (m_radius < m_minRadius) 
        m_radius = m_minRadius;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::getModelViewProjectionMatrix(mat4 mvp) noexcept
{
    if(m_modelViewNeedUpdate)
    {
        vec3 eye, view, center;
        getEye(eye);
        getNormalizedViewVector(view);
        glm_vec3_add(eye, view, center);

        glm_lookat(eye, center, (vec3){ 0.f, 1.f, 0.f }, m_modelView);
        m_modelViewNeedUpdate = false;
    }

    glm_mat4_mul(m_projection, m_modelView, mvp);
}


void OrbitCamera::getEye(vec3 eye) const noexcept
{
    const auto sineAzimuth   = sin(m_azimuth);
    const auto cosineAzimuth = cos(m_azimuth);
    const auto sinePolar     = sin(m_polar);
    const auto cosinePolar   = cos(m_polar);

    eye[0] = m_center[0] + m_radius * cosinePolar * cosineAzimuth;
    eye[1] = m_center[1] + m_radius * sinePolar;
    eye[2] = m_center[2] + m_radius * cosinePolar * sineAzimuth;
}


void OrbitCamera::getViewPoint(vec3 view) const noexcept
{
    view[0] = m_center[0];
    view[1] = m_center[1];
    view[2] = m_center[2];
}


void OrbitCamera::getNormalizedViewVector(vec3 v) const noexcept
{
    vec3 view, eye;
    getViewPoint(view);
    getEye(eye);
    glm_vec3_sub(view, eye, v);
    glm_vec3_normalize(v);
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
