#ifndef TRANSFORM_2D_HPP
#define TRANSFORM_2D_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Class for optimized use of orthographic matrix
class Transform2D
{
public:
    Transform2D() noexcept;
    virtual ~Transform2D() noexcept;

    void setPosition(float x, float y) noexcept;
    void setPosition(const glm::vec2& position) noexcept;

    void setRotation(float angle) noexcept;

    void setScale(float factorX, float factorY) noexcept;
    void setScale(const glm::vec2& factors)     noexcept;
    void setScale(float factor)                 noexcept;

    void setOrigin(float x, float y)        noexcept;
    void setOrigin(const glm::vec2& origin) noexcept;

    const glm::vec2& getPosition() const noexcept;
    float            getRotation() const noexcept;
    const glm::vec2& getScale()    const noexcept;
    const glm::vec2& getOrigin()   const noexcept;

    void move(float offsetX, float offsetY) noexcept;
    void move(const glm::vec2& offset)      noexcept;
    void rotate(float angle)                noexcept;

    void scale(float factorX, float factorY) noexcept;
    void scale(const glm::vec2& factor)      noexcept;

    const glm::mat4& getMatrix() const noexcept;

    void loadIdentity() noexcept;

private:
    mutable glm::mat4 m_matrix;
    glm::vec2         m_origin;
    glm::vec2         m_position;         
    glm::vec2         m_scale;
    float             m_rotation;      
    mutable bool      m_transformNeedUpdate;       
};

#endif 