#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include <cglm/types.h>
#include <cglm/call/mat4.h>

#include "Export.hpp"

class SHINY_API OrbitCamera
{
public:
	OrbitCamera() noexcept;

    void setup(const vec3 minPoint, const vec3 maxPoint) noexcept;

    void updateProjectionMatrix(float aspect) noexcept;
    void setDrawDistance(float distance) noexcept;

    void rotateAzimuth(float radians) noexcept;
    void rotatePolar(float radians) noexcept;
	void moveHorizontal(float distance) noexcept;
	void moveVertical(float distance) noexcept;
    void zoom(float distance) noexcept;
    
    void getModelViewProjectionMatrix(mat4 mvp) noexcept;

    void getEye(vec3 eye) const noexcept;
    void getViewPoint(vec3 point) const noexcept;
    void getNormalizedViewVector(vec3 v) const noexcept;

    float getAzimuthAngle() const noexcept;
    float getPolarAngle() const noexcept;
    float getRadius() const noexcept;

private:
    mat4         m_projection;
    mutable mat4 m_modelView;
    vec3         m_center;
    float        m_radius;
    float        m_minRadius;
    float        m_azimuth;
    float        m_polar;
    float        m_aspect;
    float        m_drawDistance;
    mutable bool m_modelViewNeedUpdate;
};

#endif // !ORBIT_CAMERA_HPP