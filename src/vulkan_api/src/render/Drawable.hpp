
#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP


class Drawable
{
public:
    virtual ~Drawable();

protected:
    virtual void draw(class RenderTarget& target, VkCommandBuffer cmd) const noexcept;

    friend class RenderTarget;
};

#endif // !DRAWABLE_HPP