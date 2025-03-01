#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include <glm/glm.hpp>

class OrbitCamera
{
public:
	OrbitCamera(const glm::vec3& center, const glm::vec3& upVector, float radius, float minRadius, float azimuthAngle, float polarAngle) noexcept;

    void rotateAzimuth(const float radians) noexcept;
    void rotatePolar(const float radians) noexcept;
	void zoom(const float distance) noexcept;
	void moveHorizontal(const float distance) noexcept;
	void moveVertical(const float distance) noexcept;
    
    glm::mat4 getModelViewProjectionMatrix() const noexcept;
    glm::vec3 getEye() const noexcept;
    glm::vec3 getViewPoint() const noexcept;
    glm::vec3 getVectorUp() const noexcept;
    glm::vec3 getNormalizedViewVector() const noexcept;

    float getAzimuthAngle() const noexcept;
    float getPolarAngle() const noexcept;
    float getRadius() const noexcept;

private:
    glm::mat4 m_projection;
    glm::mat4 m_modelView;
    glm::vec3 m_center;
    glm::vec3 m_vectorUp;
    float m_radius;
    float m_minRadius;
    float m_azimuthAngle;
    float m_polarAngle;
};

#endif // !ORBIT_CAMERA_HPP