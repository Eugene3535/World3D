#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cglm/struct/vec3.h>
#include <cglm/struct/mat4.h>
#include <cglm/struct/cam.h>
#include <cglm/util.h>

struct Camera
{
    enum Direction
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    Camera() noexcept;

    void processKeyboard(Camera::Direction direction, float deltaTime) noexcept;
    void processMouseMovement(float xoffset, float yoffset) noexcept;
    mat4s getViewMatrix() noexcept;

//  camera Attributes
    vec3s position;
    vec3s front;
    vec3s up;
    vec3s right;
    vec3s worldUp;

//  euler Angles
    float yaw;
    float pitch;

//  camera options
    float movementSpeed;
    float mouseSensitivity;
};

#endif // !CAMERA_HPP