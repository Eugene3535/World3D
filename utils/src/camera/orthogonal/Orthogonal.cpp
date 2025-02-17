#include "camera/orthogonal/Orthogonal.hpp"


Orthogonal::Orthogonal() noexcept:
    Transform2D(),
    m_projection(glm::identity<glm::mat4>()),
    m_modelViewNeedUpdate(true)
{
    
}


Orthogonal::~Orthogonal() noexcept = default;


void Orthogonal::setupProjectionMatrix(int32_t width, int32_t height) noexcept
{
    m_projection = glm::ortho(0.f, static_cast<float>(width), static_cast<float>(height), 0.f, -1.f, 1.f);
}


glm::mat4 Orthogonal::getModelViewProjectionMatrix() const noexcept
{
    return m_projection * getMatrix();
}
