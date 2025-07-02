#include <glm/gtc/constants.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "camera/orbit/OrbitCamera.hpp"


OrbitCamera::OrbitCamera() noexcept:
    m_projection(glm::identity<glm::mat4>()),
    m_modelView(glm::identity<glm::mat4>()),
    m_center(),
    m_radius(0.f),
    m_minRadius(0.f),
    m_azimuth(0.f),
    m_polar(glm::quarter_pi<float>()),
    m_aspect(0.f),
    m_drawDistance(3.f),
    m_modelViewNeedUpdate(true)
{
    
}


void OrbitCamera::setup(const glm::vec3& minPoint, const glm::vec3& maxPoint) noexcept
{
    m_center[0] = (minPoint[0] + maxPoint[0]) * 0.5f;
    m_center[1] = (minPoint[1] + maxPoint[1]) * 0.5f;
    m_center[2] = (minPoint[2] + maxPoint[2]) * 0.5f;

    m_center = (minPoint + maxPoint) * 0.5f;
    glm::vec3 half_size = (maxPoint - minPoint) * 0.5f;

    m_radius = glm::length(half_size) * 1.5f;

    m_minRadius = m_radius * 0.3f;
    m_azimuth = 0;
    m_polar = glm::quarter_pi<float>();
    m_drawDistance = m_radius * 2;
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::updateProjectionMatrix(float aspect) noexcept
{
    m_aspect = aspect;
    m_projection = glm::perspective(glm::radians(60.f), aspect, 0.1f, m_drawDistance);
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
    const auto fullCircle = glm::two_pi<float>();
    m_azimuth = fmodf(m_azimuth, fullCircle);

    if (m_azimuth < 0.f) 
        m_azimuth = fullCircle + m_azimuth;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::rotatePolar(float radians) noexcept
{
    m_polar += radians;

//  Check if the angle hasn't exceeded quarter of a circle to prevent flip, add a bit of epsilon like 0.001 radians
    const auto polarCap = glm::half_pi<float>() - 0.001f;
    m_polar = glm::clamp(m_polar, -polarCap, polarCap);
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::moveHorizontal(float distance) noexcept
{
    glm::vec3 view = getNormalizedViewVector();
    glm::vec3 strafe = glm::normalize(glm::cross(view, glm::vec3(0, 1, 0)));
    m_center += strafe * distance;

    m_modelViewNeedUpdate = true;
}


void OrbitCamera::moveVertical(float distance) noexcept
{
    const glm::vec3 worldUp = {0.f, 1.f, 0.f };

    glm::vec3 eye = getEye();
    glm::vec3 zaxis = m_center - eye;
    glm::vec3 xaxis = glm::normalize(glm::cross(zaxis, worldUp));
    glm::vec3 yaxis = glm::normalize(glm::cross(xaxis, zaxis));

    m_center += worldUp * distance;
    m_modelViewNeedUpdate = true;
}


void OrbitCamera::zoom(float distance) noexcept
{
    m_radius += distance;

    if (m_radius < m_minRadius) 
        m_radius = m_minRadius;

    m_modelViewNeedUpdate = true;
}


glm::mat4 OrbitCamera::getModelViewProjectionMatrix() const noexcept
{
    if(m_modelViewNeedUpdate)
    {
        const glm::vec3 worldUp = { 0.f, 1.f, 0.f };
        glm::vec3 eye = getEye();
        glm::vec3 view = getNormalizedViewVector();
        glm::vec3 center = eye + view;

        m_modelView = glm::lookAt(eye, center, worldUp);
        m_modelViewNeedUpdate = false;
    }

    return m_projection * m_modelView;
}


glm::vec3 OrbitCamera::getEye() const noexcept
{
    const auto sineAzimuth   = sin(m_azimuth);
    const auto cosineAzimuth = cos(m_azimuth);
    const auto sinePolar     = sin(m_polar);
    const auto cosinePolar   = cos(m_polar);

    float x = m_center.x + m_radius * cosinePolar * cosineAzimuth;
    float y = m_center.y + m_radius * sinePolar;
    float z = m_center.z + m_radius * cosinePolar * sineAzimuth;

    return { x, y, z };
}


glm::vec3 OrbitCamera::getViewPoint() const noexcept
{
    return m_center;
}


glm::vec3 OrbitCamera::getNormalizedViewVector() const noexcept
{
    return glm::normalize(m_center - getEye());
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