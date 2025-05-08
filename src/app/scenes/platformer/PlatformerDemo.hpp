#ifndef PLATFORMER_DEMO_HPP
#define PLATFORMER_DEMO_HPP

#include <memory>
#include <vector>

#include "scenes/DemoScene.hpp"


class PlatformerDemo final:
    public DemoScene
{
public:
    PlatformerDemo(class sf::Window& window) noexcept;
    ~PlatformerDemo();

    bool init() noexcept override;
    void update(const class sf::Time& dt) noexcept override;
    void draw() noexcept override;

private:
    std::unique_ptr<class Texture> m_textureGoomba;
    std::unique_ptr<class Texture> m_textureMegaman;
    std::unique_ptr<class Texture> m_textureBullet;
    std::unique_ptr<class Texture> m_textureMovingPlatform;
    std::unique_ptr<class Texture> m_textureHealthBar;

    std::unique_ptr<class ShaderProgram>     m_tilemapProgram;
    std::unique_ptr<class ShaderProgram>     m_healthbarProgram;
    std::unique_ptr<class VertexArrayObject> m_vao;

    std::unique_ptr<class OrthogonalCamera>  m_camera;
    std::unique_ptr<class GlBuffer>          m_uniformBuffer;

    std::unique_ptr<class TileMap>             m_tilemap;
    std::unique_ptr<class SpriteHolder>        m_sprites;
    std::vector<std::unique_ptr<class Entity>> m_entities;

    std::unique_ptr<class Animator> m_goomba;
    std::unique_ptr<class Animator> m_bullet;
    std::unique_ptr<class Animator> m_movingPlatrorm;
    std::unique_ptr<class Animator> m_megaman;

    std::unique_ptr<class Player> m_player;
    std::unique_ptr<class Sprite2D> m_healthbar;
};


#endif // !PLATFORMER_DEMO_HPP