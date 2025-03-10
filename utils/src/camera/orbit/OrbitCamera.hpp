#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include <glm/glm.hpp>

class OrbitCamera
{
public:
	OrbitCamera(const glm::vec3& center, float radius, float minRadius, float azimuthAngle, float polarAngle) noexcept;

    void updateProjectionMatrix(float aspect) noexcept;

    void rotateAzimuth(float radians) noexcept;
    void rotatePolar(float radians) noexcept;
	void zoom(float distance) noexcept;

	void moveHorizontal(float distance) noexcept;
	void moveVertical(float distance) noexcept;
    
    glm::mat4 getModelViewProjectionMatrix() const noexcept;

    glm::vec3 getEye() const noexcept;
    const glm::vec3& getViewPoint() const noexcept;
    glm::vec3 getNormalizedViewVector() const noexcept;

    float getAzimuthAngle() const noexcept;
    float getPolarAngle() const noexcept;
    float getRadius() const noexcept;

private:
    glm::mat4 m_projection;
    glm::mat4 m_modelView;
    glm::vec3 m_center;
    float m_radius;
    float m_minRadius;
    float m_azimuth;
    float m_polar;
    float m_fov;
    float m_aspect;
};

#endif // !ORBIT_CAMERA_HPP