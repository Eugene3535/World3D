#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>


class Camera final
{
public:
	Camera(struct GLFWwindow* window) noexcept;
    Camera(const Camera&) noexcept = delete;
    Camera(const Camera&&) noexcept = delete;
    Camera& operator = (Camera&) noexcept = delete;
    Camera& operator = (Camera&&) noexcept = delete;
	~Camera() noexcept;

    void setPosition(float x, float y, float z) noexcept;
    void setPosition(const glm::vec3& position) noexcept;

    void update(float dt) noexcept;

    const glm::vec3& getPosition() const noexcept;
    glm::mat4 getViewMatrix() const noexcept;

private:
    struct GLFWwindow* m_window;

    glm::vec3 m_eye;
    glm::vec3 m_delta;

    float m_pitch;
    float m_yaw;
    float m_velocity;
};

#endif // !CAMERA_HPP
