#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cglm/call/vec3.h>
#include <cglm/call/cam.h>

#include "camera/perspective/Camera3D.hpp"


static void Vector3RotateByAxisAngle(vec3 v, vec3 axis, float angle, vec3 result)
{
    // Using Euler-Rodrigues Formula
    // Ref.: https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula

    vec3 res;
    glm_vec3_copy(v, res);

    float length = glm_vec3_norm(axis);

    if (length == 0.f) 
        length = 1.f;

    float ilength = 1.f / length;
    axis[0] *= ilength;
    axis[1] *= ilength;
    axis[2] *= ilength;

    angle /= 2.f;
    float a = sinf(angle);
    float b = axis[0] * a;
    float c = axis[1] * a;
    float d = axis[2] * a;
    a = cosf(angle);

    vec3 w = { b, c, d };
    vec3 wv, wwv;
    glm_vec3_cross(w, v, wv);
    glm_vec3_cross(w, wv, wwv);

    a = a + a;
    glm_vec3_scale(wv, a, wv);
    glm_vec3_add(wwv, wwv, wwv);
    glm_vec3_add(res, wv, res);
    glm_vec3_add(res, wwv, res);

    glm_vec3_copy(res, result);
}


Camera3D::Camera3D() noexcept:
    m_fovy(60)
{
    glmc_vec3_zero(m_position);
    glmc_vec3_zero(m_target);
    glmc_vec3_zero(m_up);

    m_mode = Camera3D::FirstPerson;
}


void Camera3D::moveForward(float distance, bool moveInWorldPlane) noexcept
{
    vec3 forward;
    getForward(forward);

    if (moveInWorldPlane)
    {
//      Project vector onto world plane
        forward[1] = 0;
        glmc_vec3_normalize(forward);
    }

//  Scale by distance
    glmc_vec3_scale(forward, distance, forward);

//  Move position and target
    glmc_vec3_add(m_position, forward, m_position);
    glmc_vec3_add(m_target, forward, m_target);
}


void Camera3D::moveUp(float distance) noexcept
{
    vec3 up;
    getUp(up);

//  Scale by distance
    glmc_vec3_scale(up, distance, up);

//  Move position and target
    glmc_vec3_add(m_position, up, m_position);
    glmc_vec3_add(m_target, up, m_target);
}


void Camera3D::moveRight(float distance, bool moveInWorldPlane) noexcept
{
    vec3 right;
    getRight(right);

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        right[1] = 0;
        glmc_vec3_normalize(right);
    }

//  Scale by distance
    glmc_vec3_scale(right, distance, right);

//  Move position and target
    glmc_vec3_add(m_position, right, m_position);
    glmc_vec3_add(m_target, right, m_target);
}


void Camera3D::moveToTarget(float delta) noexcept
{
    float distance = glmc_vec3_distance(m_position, m_target);
    distance += delta;

//  Distance must be greater than 0
    if (distance <= 0.f) 
        distance = 0.001f;

//  Set new distance by moving the position along the forward vector
    vec3 forward;
    getForward(forward);
    glmc_vec3_scale(forward, -distance, forward);
    glmc_vec3_add(m_target, forward, m_position);
}


void Camera3D::rotateYaw(float angle, bool rotateAroundTarget) noexcept
{
    // Rotation axis
    vec3 up;
    getUp(up);

    // View vector
    vec3 targetPosition;
    glmc_vec3_sub(m_target, m_position, targetPosition);

    // Rotate view vector around up axis
    Vector3RotateByAxisAngle(targetPosition, up, angle, targetPosition);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        glm_vec3_sub(m_target, targetPosition, m_position);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        glm_vec3_add(m_position, targetPosition, m_target);
    }
}


void Camera3D::rotatePitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp) noexcept
{
    vec3 up, targetPosition;
    getUp(up);
    glm_vec3_sub(m_target, m_position, targetPosition);

    auto vec3_angle = [](vec3 v1, vec3 v2) -> float
    {
        vec3 cross;
        glm_cross(v1, v2, cross);
        float len = glm_vec3_norm(cross);
        float dot = glm_dot(v1, v2);
        
        return atan2f(len, dot);
    };

    if (lockView)
    {
        // In these camera modes we clamp the Pitch angle
        // to allow only viewing straight up or down.

        // Clamp view up
        float maxAngleUp = vec3_angle(up, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors

        if (angle > maxAngleUp) 
            angle = maxAngleUp;

        vec3 negativeUp;
        glm_vec3_negate_to(up, negativeUp);
        // Clamp view down
        float maxAngleDown = vec3_angle(negativeUp, targetPosition);
        maxAngleDown *= -1.f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors

        if (angle < maxAngleDown) 
            angle = maxAngleDown;
    }

    // Rotation axis
    vec3 right;
    getRight(right);

    // Rotate view vector around right axis
    Vector3RotateByAxisAngle(targetPosition, right, angle, targetPosition);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        glm_vec3_sub(m_target, targetPosition, m_position);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        glm_vec3_add(m_position, targetPosition, m_target);
    }

    if (rotateUp)
    {
        // Rotate up direction around right axis
        Vector3RotateByAxisAngle(m_up, right, angle, m_up);
    }
}


void Camera3D::getForward(vec3 forward) noexcept
{
    glmc_vec3_sub(m_target, m_position, forward);
    glmc_vec3_normalize(forward);
}


void Camera3D::getUp(vec3 up) noexcept
{
    glmc_vec3_normalize_to(m_up, up);
}


void Camera3D::getRight(vec3 right) noexcept
{
    vec3 forward, up;
    getForward(forward);
    getUp(up);
    glmc_vec3_crossn(forward, up, right);
}


void Camera3D::getViewMatrix(mat4 m) noexcept
{
    glmc_lookat(m_position, m_target, m_up, m);
}


void Camera3D::getProjectionMatrix(mat4 m, float aspect) noexcept
{
    glmc_perspective(glm_rad(m_fovy), aspect, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR, m);
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
        vec3 up, view;
        getUp(up);
        mat4 rotation = GLM_MAT4_IDENTITY_INIT;
        glm_rotate(rotation, cameraOrbitalSpeed, up);
        glm_vec3_sub(m_position, m_target, view);

        vec4 v = { view[0], view[1], view[2], 1.f };
        vec4 result;
        glm_mat4_mulv(rotation, v, result);
        glm_vec3(result, view);
        glm_vec3_add(m_target, view, m_position);
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