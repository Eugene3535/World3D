#ifndef CAMERA_3D_HPP
#define CAMERA_3D_HPP

#include <cglm/types.h>
#include <cglm/call/mat4.h>

#include "Export.hpp"

#define CAMERA_MOVE_SPEED     5.4f // Units per second
#define CAMERA_ROTATION_SPEED 0.03f
#define CAMERA_PAN_SPEED      0.2f

// Camera mouse movement sensitivity
#define CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f

// Camera orbital speed in CAMERA_ORBITAL mode
#define CAMERA_ORBITAL_SPEED 0.5f // Radians per second

#define CAMERA_CULL_DISTANCE_NEAR 0.01f   // Default projection matrix near cull distance
#define CAMERA_CULL_DISTANCE_FAR  1000.0f // Default projection matrix far cull distance


class SHINY_API Camera3D
{
public:
    enum Mode
    {
        Free = 0,
        Orbital, 
        FirstPerson,
        ThirdPerson 
    };

    Camera3D() noexcept;

    // Camera movement
    void moveForward(float distance, bool moveInWorldPlane) noexcept;
    void moveUp(float distance) noexcept;
    void moveRight(float distance, bool moveInWorldPlane) noexcept;
    void moveToTarget(float delta) noexcept;

    // Camera rotation
    void rotateYaw(float angle, bool rotateAroundTarget) noexcept;
    void rotatePitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp) noexcept;
 
    void getForward(vec3 forward) noexcept;
    void getUp(vec3 up)           noexcept;
    void getRight(vec3 right)     noexcept;

    void getViewMatrix(mat4 m) noexcept;
    void getProjectionMatrix(mat4 m, float aspect) noexcept;

    void update(float dx, float dy, Mode mode, float dt) noexcept;

public:
    vec3 m_position;
    vec3 m_target;  
    vec3 m_up;    

    float m_fovy;

    Mode m_mode;
};

#endif // !CAMERA_3D_HPP