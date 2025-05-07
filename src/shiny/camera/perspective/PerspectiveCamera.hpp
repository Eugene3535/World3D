#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include <cglm/types.h>
#include <cglm/call/mat4.h>

#include "Export.hpp"

class SHINY_API PerspectiveCamera
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

    PerspectiveCamera() noexcept;

    void updateProjectionMatrix(float aspect) noexcept;
    void getModelViewProjectionMatrix(mat4 mvp) noexcept;

    void setDrawDistance(float distance) noexcept;

    void setPosition(float x, float y, float z) noexcept;
    void setPosition(vec3 position) noexcept;

    void processKeyboard(Direction direction, float velocity) noexcept;
    void processMouseMovement(float xoffset, float yoffset) noexcept;
    void processMouseScroll(float delta) noexcept;

    void getPosition(vec3 position) const noexcept;

private:
    void recalculateModelViewMatrix() noexcept;

    mat4 m_projection;
    mat4 m_modelView;

    vec3 m_eye;
    vec3 m_vectorFront;
    vec3 m_vectorUp;
    vec3 m_vectorRight;

    float m_yaw;
    float m_pitch;
    float m_fov;
    float m_aspect;
    float m_drawDistance;

    bool m_modelViewNeedUpdate;
};

#endif // !PERSPECTIVE_CAMERA_HPP