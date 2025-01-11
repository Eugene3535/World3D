#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

#include "Export.hpp"

class RE_API Camera final
{
public:
	Camera(void* handle) noexcept;
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
    void* m_handle;

    glm::vec3 m_eye;
    glm::vec3 m_delta;

    float m_pitch;
    float m_yaw;
    float m_velocity;
};

#endif // !CAMERA_HPP
