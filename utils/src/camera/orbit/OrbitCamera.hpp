#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include <glm/glm.hpp>

class OrbitCamera
{
public:
	OrbitCamera() noexcept;

    void setup(const glm::vec3& minPoint, const glm::vec3& maxPoint) noexcept;

    void updateProjectionMatrix(float aspect) noexcept;
    void setDrawDistance(float distance) noexcept;

    void rotateAzimuth(float radians) noexcept;
    void rotatePolar(float radians) noexcept;
	void moveHorizontal(float distance) noexcept;
	void moveVertical(float distance) noexcept;
    void zoom(float distance) noexcept;
    
    glm::mat4 getModelViewProjectionMatrix() const noexcept;

    glm::vec3 getEye() const noexcept;
    const glm::vec3& getViewPoint() const noexcept;
    glm::vec3 getNormalizedViewVector() const noexcept;

    float getAzimuthAngle() const noexcept;
    float getPolarAngle() const noexcept;
    float getRadius() const noexcept;

private:
    glm::mat4 m_projection;
    mutable glm::mat4 m_modelView;
    glm::vec3 m_center;
    float m_radius;
    float m_minRadius;
    float m_azimuth;
    float m_polar;
    float m_aspect;
    float m_drawDistance;
    mutable bool m_modelViewNeedUpdate;
};

#endif // !ORBIT_CAMERA_HPP