#include "camera/Camera.hpp"


// Default camera values
static const float YAW         = -90.0f;
static const float PITCH       =  0.0f;
static const float SPEED       =  2.5f;
static const float SENSITIVITY =  0.1f;


static void update_camera_vectors(Camera* camera)
{
    // calculate the new Front vector
    vec3s front;
    front.x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    front.y = sin(glm_rad(camera->pitch));
    front.z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    camera->front = glms_vec3_normalize(front);
    
    // also re-calculate the Right and Up vector
    camera->right = glms_vec3_normalize(glms_cross(camera->front, camera->worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    camera->up    = glms_cross(camera->right, camera->front);
}



Camera::Camera() noexcept
{
    position = {0.f, 0.f,  3.f};
    front    = {0.f, 0.f, -1.f};
    up       = {0.f, 1.f,  0.f};
    right    = {0.f, 0.f,  0.f};
    worldUp  = {0.f, 1.f,  0.f};

    yaw   = YAW;
    pitch = PITCH;

    movementSpeed    = SPEED; 
    mouseSensitivity = SENSITIVITY; 

    update_camera_vectors(this);
}


void Camera::processKeyboard(Camera::Direction direction, float deltaTime) noexcept
{
    float velocity = movementSpeed * deltaTime;

    switch (direction)
    {
        case Camera::FORWARD:  glm_vec3_muladds(front.raw, velocity, position.raw); break;
        case Camera::BACKWARD: glm_vec3_mulsubs(front.raw, velocity, position.raw); break;
        case Camera::LEFT:     glm_vec3_mulsubs(right.raw, velocity, position.raw); break;
        case Camera::RIGHT:    glm_vec3_muladds(right.raw, velocity, position.raw); break;
        
        default:
            break;
    }
}


void Camera::processMouseMovement(float xoffset, float yoffset) noexcept
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

//  make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.f)
        pitch = 89.f;

    if (pitch < -89.f)
        pitch = -89.f;

//  update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors(this);
}


mat4s Camera::getViewMatrix() noexcept
{
    vec3s center = glms_vec3_add(position, front);

    return glms_lookat(position, center, up);
}