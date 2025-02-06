#ifndef ORTHOGONAL_HPP
#define ORTHOGONAL_HPP

#include "graphics/Transform2D.hpp"

class OGL_API Orthogonal final:
    public Transform2D
{
public:
	Orthogonal() noexcept;
	~Orthogonal() noexcept;

    void      setupProjectionMatrix(int32_t width, int32_t height) noexcept;
    glm::mat4 getModelViewProjectionMatrix() const noexcept;

private:
    glm::mat4 m_projection;
    bool      m_modelViewNeedUpdate;
};

#endif // !ORTHOGONAL_HPP