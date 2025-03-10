#include <glm/gtc/matrix_transform.hpp>

#include "camera/orbit/OrbitCamera.hpp"


OrbitCamera::OrbitCamera(const glm::vec3& center, float radius, float minRadius, float azimuthAngle, float polarAngle) noexcept:
    m_projection(glm::identity<glm::mat4>()),
    m_modelView(glm::identity<glm::mat4>()),
    m_center(center),
    m_radius(radius),
    m_minRadius(minRadius),
    m_azimuth(azimuthAngle),
    m_polar(polarAngle),
    m_fov(45.0f),
    m_aspect(0.0f)
{

}


void OrbitCamera::updateProjectionMatrix(float aspect) noexcept
{
    m_aspect = aspect;
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, 0.1f, 300.f);
}


void OrbitCamera::rotateAzimuth(float radians) noexcept
{
    m_azimuth += radians;

    // Keep azimuth angle within range <0..2PI) - it's not necessary, just to have it nicely output
    constexpr auto fullCircle = glm::pi<float>() * 2.f;
    m_azimuth = fmodf(m_azimuth, fullCircle);

    if (m_azimuth < 0.f) 
        m_azimuth = fullCircle + m_azimuth;
}


void OrbitCamera::rotatePolar(float radians) noexcept
{
    m_polar += radians;

    // Check if the angle hasn't exceeded quarter of a circle to prevent flip, add a bit of epsilon like 0.001 radians
    constexpr auto polarCap = glm::pi<float>() / 2.0f - 0.001f;
    m_polar = glm::clamp(m_polar, -polarCap, polarCap);
}


void OrbitCamera::zoom(float distance) noexcept
{
    m_radius += distance;

    if (m_radius < m_minRadius) 
        m_radius = m_minRadius;
}


void OrbitCamera::moveHorizontal(float distance) noexcept
{
    const auto position = getEye();
    const glm::vec3 viewVector = getNormalizedViewVector();
    const glm::vec3 strafeVector = glm::normalize(glm::cross(viewVector, glm::vec3(0.f, 1.f, 0.f)));
    m_center += strafeVector * distance;
}


void OrbitCamera::moveVertical(float distance) noexcept
{
    auto front = getViewPoint() - getEye();
    auto right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    auto up = glm::normalize(glm::cross(right, front));

    m_center += up * distance;
}


glm::mat4 OrbitCamera::getModelViewProjectionMatrix() const noexcept
{
    const auto eye = getEye();

	return m_projection * glm::lookAt(eye, eye + getNormalizedViewVector(), glm::vec3(0.f, 1.f, 0.f));
}


glm::vec3 OrbitCamera::getEye() const noexcept
{
    const auto sineAzimuth   = sin(m_azimuth);
    const auto cosineAzimuth = cos(m_azimuth);
    const auto sinePolar     = sin(m_polar);
    const auto cosinePolar   = cos(m_polar);

    const auto ex = m_center.x + m_radius * cosinePolar * cosineAzimuth;
    const auto ey = m_center.y + m_radius * sinePolar;
    const auto ez = m_center.z + m_radius * cosinePolar * sineAzimuth;

    return glm::vec3(ex, ey, ez);
}


const glm::vec3& OrbitCamera::getViewPoint() const noexcept
{
    return m_center;
}


glm::vec3 OrbitCamera::getNormalizedViewVector() const noexcept
{
    return glm::normalize(getViewPoint() - getEye());
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
