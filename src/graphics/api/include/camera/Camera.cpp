#include <cglm/struct/cam.h>
#include <cglm/util.h>

#include "camera/Camera.hpp"


Camera::Camera() noexcept : yaw(0.f), pitch(0.f), movementSpeed(2.5f), mouseSensitivity(0.1f)
{
    position = {0.f, 2.f, 3.f};
    front    = {0.f, 0.f, -1.f};
    up       = {0.f, 1.f, 0.f};
    right    = {0.f, 0.f, 0.f};
    worldUp  = {0.f, 1.f, 0.f};

    updateCameraVectors();
}


void Camera::processKeyboard(Camera::Direction direction, float deltaTime) noexcept
{
    float velocity = movementSpeed * deltaTime;

    switch (direction)
    {
        case Camera::FORWARD:
            glm_vec3_muladds(front.raw, velocity, position.raw);
            break;
        case Camera::BACKWARD:
            glm_vec3_mulsubs(front.raw, velocity, position.raw);
            break;
        case Camera::LEFT:
            glm_vec3_mulsubs(right.raw, velocity, position.raw);
            break;
        case Camera::RIGHT:
            glm_vec3_muladds(right.raw, velocity, position.raw);
            break;

        default:
            break;
    }
}


void Camera::processMouseMovement(float xoffset, float yoffset) noexcept
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    //  make sure that when pitch is out of bounds, screen doesn't get flipped
    yaw = fmod(yaw, 360.f);

    if (yaw < 0.f)
        yaw += 360.f;

    pitch = glm_clamp(pitch, -89.f, 89.f);

    //  update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}


mat4s Camera::getViewMatrix() noexcept
{
    vec3s center = glms_vec3_add(position, front);

    return glms_lookat(position, center, up);
}


void Camera::updateCameraVectors() noexcept
{
    // calculate the new Front vector
    vec3s dir;
    dir.x = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
    dir.y = sin(glm_rad(pitch));
    dir.z = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
    front = glms_vec3_normalize(dir);

    // also re-calculate the Right and Up vector
    right = glms_vec3_normalize(
        glms_cross(front, worldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glms_cross(right, front);
}
