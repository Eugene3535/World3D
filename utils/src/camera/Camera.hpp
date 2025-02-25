#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//const float YAW         = -90.0f;
//const float PITCH       =  0.0f;
//const float SPEED       =  2.5f;
//const float SENSITIVITY =  0.1f;
//const float ZOOM        =  45.0f;

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

    glm::mat4 getModelViewProjectionMatrix() const noexcept;

    void processKeyboard(Direction direction, float deltaTime) noexcept;
    void processMouseMovement(float xoffset, float yoffset) noexcept;
    void processMouseScroll(float delta) noexcept;

private:
    void recalculateModelViewMatrix() noexcept;

    glm::mat4 m_projection;
    glm::mat4 m_modelView;

    glm::vec3 m_eye;
    glm::vec3 m_vectorFront;
    glm::vec3 m_vectorUp;
    glm::vec3 m_vectorRight;

    float m_yaw;
    float m_pitch;
    float m_fov;

    float m_velocity;
    float m_mouseSensitivity;
};

#endif // !CAMERA_HPP