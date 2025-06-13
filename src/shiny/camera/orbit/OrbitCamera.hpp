#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include <cglm/types.h>
#include <cglm/call/mat4.h>

#include "Export.hpp"

class SHINY_API OrbitCamera
{
public:
    enum Direction
    {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down
    };

	OrbitCamera() noexcept;

    void focusOn(vec3 target) noexcept;
    void setPosition(vec3 eye) noexcept;

    void rotateAroundTarget(float dx, float dy) noexcept;
    void movePanoramic(float dx, float dy) noexcept;

    void move(Direction direction, float distance) noexcept;
    
    void getModelViewMatrix(mat4 m) noexcept;

    void getEye(vec3 eye)       const noexcept;
    void getTarget(vec3 target) const noexcept;
    void getViewVector(vec3 v)  const noexcept;

    float getAzimuth() const noexcept;
    float getPolar()   const noexcept;
    float getRadius()  const noexcept;

private:
    mutable mat4  m_modelView;
    mutable vec3  m_eye;
    mutable vec3  m_target;
    mutable float m_radius;

    float        m_azimuth;
    float        m_polar;
    mutable bool m_modelViewNeedUpdate;
};

#endif // !ORBIT_CAMERA_HPP