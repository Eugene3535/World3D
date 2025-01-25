#ifndef ORTHOGONAL_HPP
#define ORTHOGONAL_HPP

#include "utils/Transform2D.hpp"
#include "opengl/resources/buffers/UniformBuffer.hpp"

class RE_API Orthogonal final:
    public Transform2D
{
public:
	Orthogonal(UniformBuffer biffer) noexcept;
	~Orthogonal() noexcept;

    void setupProjectionMatrix(int32_t width, int32_t height) noexcept;
    void apply() noexcept;

    glm::mat4 getModelViewProjectionMatrix() const noexcept;

private:
    glm::mat4 m_projection;

    UniformBuffer m_uniformBuffer;

    bool m_modelViewNeedUpdate;
};

#endif // !ORTHOGONAL_HPP