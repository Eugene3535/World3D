#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "camera/perspective/Camera3D.hpp"


static glm::vec3 Vector3RotateByAxisAngle(glm::vec3 v, glm::vec3 axis, float angle)
{
    // Using Euler-Rodrigues Formula
    // Ref.: https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula

    glm::vec3 result = v;

    float length = glm::length(axis);

    if (length == 0.f) 
        length = 1.f;

    float ilength = 1.0f / length;
    axis.x *= ilength;
    axis.y *= ilength;
    axis.z *= ilength;

    angle /= 2.f;
    float a = sinf(angle);
    float b = axis.x*a;
    float c = axis.y*a;
    float d = axis.z*a;
    a = cosf(angle);

    glm::vec3 w = { b, c, d };
    glm::vec3 wv = glm::cross(w, v);
    glm::vec3 wwv = glm::cross(w, wv);

    a = a + a;
    wv *= a;
    wwv = wwv + wwv;
    result += wv;
    result += wwv;

    return result;
}


Camera3D::Camera3D() noexcept:
    m_position(0),
    m_target(0),  
    m_up(0, 1, 0),    
    m_fovy(60),
    m_mode(Camera3D::FirstPerson)
{
    
}


void Camera3D::moveForward(float distance, bool moveInWorldPlane) noexcept
{
    glm::vec3 forward = getForward();

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        forward.y = 0;
        forward = glm::normalize(forward);
    }

    // Scale by distance
    forward = forward * distance;

    // Move position and target
    m_position = m_position + forward;
    m_target   = m_target + forward;
}


void Camera3D::moveUp(float distance) noexcept
{
    glm::vec3 up = getUp();

    // Scale by distance
    up *= distance;

    // Move position and target
    m_position = m_position + up;
    m_target   = m_target + up;
}


void Camera3D::moveRight(float distance, bool moveInWorldPlane) noexcept
{
    glm::vec3 right = getRight();

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        right.y = 0;
        right = glm::normalize(right);
    }

    // Scale by distance
    right *= distance;

    // Move position and target
    m_position = m_position + right;
    m_target   = m_target + right;
}


void Camera3D::moveToTarget(float delta) noexcept
{
    float distance = glm::distance(m_position, m_target);

    // Apply delta
    distance += delta;

    // Distance must be greater than 0
    if (distance <= 0) 
        distance = 0.001f;

    // Set new distance by moving the position along the forward vector
    glm::vec3 forward = getForward();
    m_position = m_target + forward * (-distance);
}


void Camera3D::rotateYaw(float angle, bool rotateAroundTarget) noexcept
{
    // Rotation axis
    glm::vec3 up = getUp();

    // View vector
    glm::vec3 targetPosition = m_target - m_position;

    // Rotate view vector around up axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, up, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        m_position = m_target - targetPosition;
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        m_target = m_position + targetPosition;
    }
}


void Camera3D::rotatePitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp) noexcept
{
    glm::vec3 up = getUp();
    glm::vec3 targetPosition = m_target - m_position;

    auto vec3_angle = [](const glm::vec3& v1, const glm::vec3& v2) -> float
    {
        glm::vec3 cross = glm::cross(v1, v2);
        float len = glm::length(cross);
        float dot = glm::dot(v1, v2);
        
        return std::atan2(len, dot);
    };

    if (lockView)
    {
        // In these camera modes we clamp the Pitch angle
        // to allow only viewing straight up or down.

        // Clamp view up
        float maxAngleUp = vec3_angle(up, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (angle > maxAngleUp) angle = maxAngleUp;

        // Clamp view down
        float maxAngleDown = vec3_angle(-up, targetPosition);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (angle < maxAngleDown) angle = maxAngleDown;
    }

    // Rotation axis
    glm::vec3 right = getRight();

    // Rotate view vector around right axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, right, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        m_position = m_target - targetPosition;
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        m_target = m_position + targetPosition;
    }

    if (rotateUp)
    {
        // Rotate up direction around right axis
        up = Vector3RotateByAxisAngle(up, right, angle);
    }
}


glm::vec3 Camera3D::getForward() const noexcept
{
   return glm::normalize(m_target - m_position);
}


glm::vec3 Camera3D::getUp() const noexcept
{
    return glm::normalize(m_up);
}


glm::vec3 Camera3D::getRight() const noexcept
{
    glm::vec3 forward = getForward();
    glm::vec3 up = getUp();

    return glm::normalize(glm::cross(forward, up));
}


glm::mat4 Camera3D::getViewMatrix() const noexcept
{
    return glm::lookAt(m_position, m_target, m_up);
}


glm::mat4 Camera3D::getProjectionMatrix(float aspect) const noexcept
{
    return glm::perspective(glm::radians(m_fovy), aspect, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
}


void Camera3D::update(float dx, float dy, Camera3D::Mode mode, float dt) noexcept
{
    bool moveInWorldPlane = ((mode == Camera3D::FirstPerson) || (mode == Camera3D::ThirdPerson));
    bool rotateAroundTarget = ((mode == Camera3D::ThirdPerson) || (mode == Camera3D::Orbital));
    bool lockView = ((mode == Camera3D::Free) || (mode == Camera3D::FirstPerson) || (mode == Camera3D::ThirdPerson) || (mode == (mode == Camera3D::Orbital)));
    bool rotateUp = false;

    // Camera speeds based on frame time
    float cameraMoveSpeed     = CAMERA_MOVE_SPEED * dt;
    float cameraRotationSpeed = CAMERA_ROTATION_SPEED * dt;
    float cameraPanSpeed      = CAMERA_PAN_SPEED * dt;
    float cameraOrbitalSpeed  = CAMERA_ORBITAL_SPEED * dt;

    if (mode == Camera3D::Orbital)
    {
        // Orbital can just orbit 
        glm::mat4 rotation = glm::rotate(glm::identity<glm::mat4>(), cameraOrbitalSpeed, getUp());
        glm::vec3 view = m_position - m_target;
        view = glm::vec3(rotation * glm::vec4(view, 1.f));
        m_position = m_target + view;
    }
    else
    {
        // Camera rotation
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  rotatePitch(-cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    rotatePitch(cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) rotateYaw(-cameraRotationSpeed, rotateAroundTarget);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  rotateYaw(cameraRotationSpeed, rotateAroundTarget);

        // Camera movement
        // Camera pan (for Camera3D::Free)
        if ((mode == Camera3D::Free) && (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)))
        {
            if (dx > 0.f) moveRight(cameraPanSpeed, moveInWorldPlane);
            if (dx < 0.f) moveRight(-cameraPanSpeed, moveInWorldPlane);
            if (dy > 0.f) moveUp(-cameraPanSpeed);
            if (dy < 0.f) moveUp(cameraPanSpeed);
        }
        else
        {
            // Mouse support
            rotateYaw(-dx * CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
            rotatePitch(-dy * CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);
        }

        // Keyboard support
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) moveForward(cameraMoveSpeed, moveInWorldPlane);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) moveRight(-cameraMoveSpeed, moveInWorldPlane);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) moveForward(-cameraMoveSpeed, moveInWorldPlane);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveRight(cameraMoveSpeed, moveInWorldPlane);

        if (mode == Camera3D::Free)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))    moveUp(cameraMoveSpeed);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) moveUp(-cameraMoveSpeed);
        }
    }

    // if ((mode == Camera3D::ThirdPerson) || (mode == Camera3D::Orbital) || (mode == Camera3D::Free))
    // {
    //     // Zoom target distance
    //     // CameraMoveToTarget(camera, -GetMouseWheelMove());
    //     if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT)) CameraMoveToTarget(camera, 2.0f);
    //     if (glfwGetKey(window, GLFW_KEY_KP_ADD)) CameraMoveToTarget(camera, -2.0f);
    // }
}