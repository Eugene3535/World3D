#include <algorithm>

#include <GLFW//glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"


Camera::Camera(void* handle) noexcept:
	m_handle(handle),
    m_eye(),
    m_delta(),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_velocity(50.0f)
{
    glfwSetInputMode(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


Camera::~Camera() noexcept = default;


void Camera::setPosition(float x, float y, float z) noexcept
{
    m_eye = { x, y, z };
}


void Camera::setPosition(const glm::vec3& position) noexcept
{
    m_eye = position;
}


void Camera::update(float dt) noexcept
{
    auto window = reinterpret_cast<GLFWwindow*>(m_handle);

    double xpos, ypos;
    int xt, yt, w, h;

    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowPos(window, &xt, &yt);
    glfwGetWindowSize(window, &w, &h);

    xt += w >> 1;
    yt += h >> 1;

    m_pitch += (xt - xpos) * 0.125f; // division by 8 is sensitivity
    m_yaw += (yt - ypos) * 0.125f;
    m_yaw = std::clamp(m_yaw, -89.0f, 89.0f);

    glfwSetCursorPos(window, xt, yt);

    m_eye.x += m_delta.x * dt;
    m_eye.z += m_delta.z * dt;
    m_delta.x = 0.0f;
    m_delta.z = 0.0f;

    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    if (is_key_pressed(GLFW_KEY_W))
    {
        m_delta.x = -sin(glm::radians(m_pitch)) * m_velocity;
        m_delta.z = -cos(glm::radians(m_pitch)) * m_velocity;
    }

    if (is_key_pressed(GLFW_KEY_S))
    {
        m_delta.x = sin(glm::radians(m_pitch)) * m_velocity;
        m_delta.z = cos(glm::radians(m_pitch)) * m_velocity;
    }

    if (is_key_pressed(GLFW_KEY_D))
    {
        m_delta.x = sin(glm::radians(m_pitch) + glm::half_pi<float>()) * m_velocity;
        m_delta.z = cos(glm::radians(m_pitch) + glm::half_pi<float>()) * m_velocity;
    }

    if (is_key_pressed(GLFW_KEY_A))
    {
        m_delta.x = sin(glm::radians(m_pitch) - glm::half_pi<float>()) * m_velocity;
        m_delta.z = cos(glm::radians(m_pitch) - glm::half_pi<float>()) * m_velocity;
    }
}


const glm::vec3& Camera::getPosition() const noexcept
{
    return m_eye;
}


glm::mat4 Camera::getViewMatrix() const noexcept
{
    const float tx = m_eye.x - sin(glm::radians(m_pitch));
    const float ty = m_eye.y + tan(glm::radians(m_yaw));
    const float tz = m_eye.z - cos(glm::radians(m_pitch));

    return glm::lookAt(m_eye, glm::vec3(tx, ty, tz), glm::vec3(0, 1, 0));
}