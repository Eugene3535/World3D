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

    void setDrawDistance(float distance) noexcept;

    void rotateAzimuth(float degrees) noexcept;
    void rotatePolar(float degrees) noexcept;
	void moveHorizontal(float distance) noexcept;
	void moveVertical(float distance) noexcept;
    void zoom(float distance) noexcept;
    
    void getModelViewMatrix(mat4 m) noexcept;

    void getEye(vec3 eye) const noexcept;
    void getTarget(vec3 target) const noexcept;
    void getNormalizedViewVector(vec3 v) const noexcept;

    float getAzimuthAngle() const noexcept;
    float getPolarAngle() const noexcept;
    float getRadius() const noexcept;

private:
    mutable mat4 m_modelView;
    mutable vec3 m_target;
    float        m_radius;
    float        m_azimuth;
    float        m_polar;
    mutable bool m_modelViewNeedUpdate;
};

#endif // !ORBIT_CAMERA_HPP