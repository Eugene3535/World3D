#include <algorithm>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <GLFW//glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

Camera::Camera(GLFWwindow* window) noexcept:
	m_window(window),
    m_eye(),
    m_delta(),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_velocity(50.0f)
{
#ifdef _WIN32
	ShowCursor(FALSE);
#endif
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
    POINT mousexy;
    GetCursorPos(&mousexy);

    int xt, yt, w, h;
    glfwGetWindowPos(m_window, &xt, &yt);
    glfwGetWindowSize(m_window, &w, &h);

    xt += w / 2;
    yt += h / 2;

    m_pitch += (xt - mousexy.x) * 0.125f; // division by 8 — sensitivity
    m_yaw += (yt - mousexy.y) * 0.125f;
    m_yaw = std::clamp(m_yaw, -89.0f, 89.0f);

    SetCursorPos(xt, yt);

    m_eye.x += m_delta.x * dt;
    m_eye.z += m_delta.z * dt;
    m_delta.x = 0.0f;
    m_delta.z = 0.0f;

    auto is_key_pressed = [this](int32_t key)
    {
        return glfwGetKey(m_window, key) == GLFW_PRESS;
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