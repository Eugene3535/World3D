#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "camera/orbit/OrbitCamera.hpp"


OrbitCamera::OrbitCamera(const glm::vec3& center, const glm::vec3& upVector, float radius, float minRadius, float azimuthAngle, float polarAngle) noexcept:
    m_projection(glm::identity<glm::mat4>()),
    m_modelView(glm::identity<glm::mat4>()),
    m_center(center), 
    m_vectorUp(upVector), 
    m_radius(radius), 
    m_minRadius(minRadius), 
    m_azimuthAngle(azimuthAngle), 
    m_polarAngle(polarAngle)
{
    m_projection = glm::perspective(glm::radians(45.f), 1200 / (float)800, 0.1f, 200.f);
}


void OrbitCamera::rotateAzimuth(const float radians) noexcept
{
    m_azimuthAngle += radians;

    constexpr auto fullCircle = 2.0f * glm::pi<float>();
    m_azimuthAngle = fmodf(m_azimuthAngle, fullCircle);

    if (m_azimuthAngle < 0.0f) 
        m_azimuthAngle = fullCircle + m_azimuthAngle;
}


void OrbitCamera::rotatePolar(const float radians) noexcept
{
    m_polarAngle += radians;
    constexpr float polarCap = glm::pi<float>() / 2.0f - 0.001f;
    m_polarAngle = std::clamp(m_polarAngle, -polarCap, polarCap);
}


void OrbitCamera::zoom(const float distance) noexcept
{
    m_radius += distance;

    if (m_radius < m_minRadius) 
        m_radius = m_minRadius;
}


void OrbitCamera::moveHorizontal(const float distance) noexcept
{
    const auto position = getEye();
    const glm::vec3 viewVector = getNormalizedViewVector();
    const glm::vec3 strafeVector = glm::normalize(glm::cross(viewVector, m_vectorUp));
    m_center += strafeVector * distance;
}


void OrbitCamera::moveVertical(const float distance) noexcept
{
    m_center += m_vectorUp * distance;
}


glm::mat4 OrbitCamera::getModelViewProjectionMatrix() const noexcept
{
    const auto eye = getEye();

	return m_projection * glm::lookAt(eye, eye + getNormalizedViewVector(), m_vectorUp);
}


glm::vec3 OrbitCamera::getEye() const noexcept
{
    const auto sineAzimuth = sin(m_azimuthAngle);
    const auto cosineAzimuth = cos(m_azimuthAngle);
    const auto sinePolar = sin(m_polarAngle);
    const auto cosinePolar = cos(m_polarAngle);

    const auto x = m_center.x + m_radius * cosinePolar * cosineAzimuth;
    const auto y = m_center.y + m_radius * sinePolar;
    const auto z = m_center.z + m_radius * cosinePolar * sineAzimuth;

    return glm::vec3(x, y, z);
}


glm::vec3 OrbitCamera::getViewPoint() const noexcept
{
    return m_center;
}


glm::vec3 OrbitCamera::getVectorUp() const noexcept
{
    return m_vectorUp;
}


glm::vec3 OrbitCamera::getNormalizedViewVector() const noexcept
{
    return glm::normalize(getViewPoint() - getEye());
}


float OrbitCamera::getAzimuthAngle() const noexcept
{
    return m_azimuthAngle;
}


float OrbitCamera::getPolarAngle() const noexcept
{
    return m_polarAngle;
}


float OrbitCamera::getRadius() const noexcept
{
    return m_radius;
}
