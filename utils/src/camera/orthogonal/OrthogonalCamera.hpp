#ifndef ORTHOGONAL_CAMERA_HPP
#define ORTHOGONAL_CAMERA_HPP

#include "graphics/Transform2D.hpp"

class OrthogonalCamera final:
    public Transform2D
{
public:
    OrthogonalCamera() noexcept;

    void      setupProjectionMatrix(int32_t width, int32_t height) noexcept;
    glm::mat4 getModelViewProjectionMatrix() const noexcept;

private:
    glm::mat4 m_projection;
    bool      m_modelViewNeedUpdate;
};

#endif // !ORTHOGONAL_CAMERA_HPP