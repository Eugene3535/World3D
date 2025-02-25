#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
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

    Camera() noexcept;

    void updateProjectionMatrix(float aspect) noexcept;
    glm::mat4 getModelViewProjectionMatrix() const noexcept;

    void setDrawDistance(float distance) noexcept;

    void setPosition(float x, float y, float z) noexcept;
    void setPosition(const glm::vec3& position) noexcept;

    void processKeyboard(Direction direction, float velocity) noexcept;
    void processMouseMovement(float xoffset, float yoffset) noexcept;
    void processMouseScroll(float delta) noexcept;

    const glm::vec3& getPosition() const noexcept;

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
    float m_aspect;
    float m_drawDistance;
};

#endif // !CAMERA_HPP