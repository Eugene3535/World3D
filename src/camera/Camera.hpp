#pragma once

#include <cglm/struct/mat4.h>
#include <cglm/struct/vec3.h>


class Camera
{
public:
    enum Direction
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    Camera() noexcept;

    void  processKeyboard(Direction direction, float deltaTime) noexcept;
    void  processMouseMovement(float xoffset, float yoffset) noexcept;
    mat4s getViewMatrix() noexcept;


    void updateCameraVectors() noexcept;

    vec3s position;
    vec3s front;
    vec3s up;
    vec3s right;
    vec3s worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
};