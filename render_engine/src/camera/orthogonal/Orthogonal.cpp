#include <glm/gtc/type_ptr.hpp>

#include "camera/orthogonal/Orthogonal.hpp"


Orthogonal::Orthogonal(UniformBuffer buffer) noexcept:
    Transform2D(),
    m_projection(glm::identity<glm::mat4>()),
    m_uniformBuffer(buffer),
    m_modelViewNeedUpdate(true)
{
   
}


Orthogonal::~Orthogonal() noexcept = default;


void Orthogonal::setupProjectionMatrix(int32_t width, int32_t height) noexcept
{
    m_projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
}


void Orthogonal::apply() noexcept
{
    m_uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(getModelViewProjectionMatrix())));
}


glm::mat4 Orthogonal::getModelViewProjectionMatrix() const noexcept
{
    return m_projection * getMatrix();
}
