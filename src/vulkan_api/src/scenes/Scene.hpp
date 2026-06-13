#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>


class Scene
{
public:
    explicit Scene(const Scene* parent) noexcept;
    ~Scene();

    virtual bool create() noexcept;

    template<class T>
    bool addScene() noexcept;

    bool isRoot() const noexcept;

protected:
    const Scene* m_parent;
    std::vector<std::unique_ptr<Scene>> m_scenes;
};

#endif // !SCENE_HPP