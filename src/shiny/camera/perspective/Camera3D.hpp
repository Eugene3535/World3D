#ifndef CAMERA_3D_HPP
#define CAMERA_3D_HPP

#include <glm/vec3.hpp> 
#include <glm/mat4x4.hpp>

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

    void setPosition(const glm::vec3& position) noexcept;
    void focusOn(const glm::vec3& target)       noexcept;
    void setWorldUp(const glm::vec3& up)        noexcept;
    void setMode(Mode mode)                     noexcept;

    // Camera movement
    void moveForward(float distance, bool moveInWorldPlane) noexcept;
    void moveUp(float distance) noexcept;
    void moveRight(float distance, bool moveInWorldPlane) noexcept;
    void moveToTarget(float delta) noexcept;

    // Camera rotation
    void rotateYaw(float angle, bool rotateAroundTarget) noexcept;
    void rotatePitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp) noexcept;

    glm::mat4 getViewMatrix() const noexcept;
    glm::mat4 getProjectionMatrix(float aspect) const noexcept;

    const glm::vec3& getPosition() const noexcept;
    const glm::vec3& getTarget()   const noexcept;
    Mode getMode()                 const noexcept;

    void update(float dx, float dy, Mode mode, float dt) noexcept;

private:
    glm::vec3 getForward() const noexcept;
    glm::vec3 getRight()   const noexcept;
    
    glm::vec3 m_position;
    glm::vec3 m_target;  
    glm::vec3 m_up;    

    float m_fovy;

    Mode m_mode;
};

#endif // !CAMERA_3D_HPP