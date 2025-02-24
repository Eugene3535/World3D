#ifndef PERSPECTIVE_HPP
#define PERSPECTIVE_HPP

#include <glm/glm.hpp>

class Perspective final
{
public:
	Perspective() noexcept;

    void setupProjectionMatrix(float fovy, float aspect, float zNear, float zFar) noexcept;
    void apply() noexcept;

    void setPosition(float x, float y, float z) noexcept;
    void setPosition(const glm::vec3& position) noexcept;

    void rotateX(float pitch) noexcept;
    void rotateY(float yaw)   noexcept;

    void moveForward(float velocity)  noexcept;
    void moveBackward(float velocity) noexcept;
    void moveLeft(float velocity)     noexcept;
    void moveRight(float velocity)    noexcept;
    void moveUp(float velocity)       noexcept;
    void moveDown(float velocity)     noexcept;

    void zoom(float delta) noexcept;
    void revertToOrigin(float height) noexcept;

    const glm::vec3& getPosition()    const noexcept;
    glm::vec3        getLineOfSight() const noexcept;
    glm::mat4 getModelViewProjectionMatrix() const noexcept;

private:
    void recalculateModelViewMatrix() noexcept;

    glm::mat4 m_projection;
    glm::mat4 m_modelView;

    glm::vec3 m_eye;
    glm::vec3 m_delta;

    float m_pitch;
    float m_yaw;

    bool m_modelViewNeedUpdate;
};

#endif // !PERSPECTIVE_HPP