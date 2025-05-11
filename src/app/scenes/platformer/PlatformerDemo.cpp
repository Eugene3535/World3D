#include <array>
#include <memory>

#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "resources/shaders/ShaderProgram.hpp"
#include "resources/holder/GlResourceHolder.hpp"
#include "camera/orthogonal/OrthogonalCamera.hpp"
#include "scenes/platformer/tilemap/TileMap.hpp"
#include "scenes/platformer/sprites/SpriteHolder.hpp"
#include "scenes/platformer/entities/Goomba.hpp"
#include "scenes/platformer/entities/Bullet.hpp"
#include "scenes/platformer/entities/MovingPlatform.hpp"
#include "scenes/platformer/entities/Player.hpp"
#include "scenes/platformer/PlatformerDemo.hpp"


#define HEALTH_BAR "health_bar"


PlatformerDemo::PlatformerDemo(sf::Window& window) noexcept:
    DemoScene(window)
{

}


PlatformerDemo::~PlatformerDemo()
{

}


bool PlatformerDemo::init() noexcept
{
    auto& holder = *GlResourceHolder::getInstance();

    auto [width, height] = m_window.getSize();

    const auto buffers = holder.create<GlBuffer, 2>();

    m_uniformBuffer = std::make_unique<GlBuffer>(buffers[0], GL_UNIFORM_BUFFER);
    m_uniformBuffer->create(sizeof(mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    m_uniformBuffer->bindBufferRange(0, 0, sizeof(mat4));

    m_camera = std::make_unique<OrthogonalCamera>(); 
    m_camera->setupProjectionMatrix(width, height);

    std::array<Shader, 2> shaders;
    if (!shaders[0].loadFromFile(FileProvider::findPathToFile("tilemap.vert"), GL_VERTEX_SHADER))   return false;
    if (!shaders[1].loadFromFile(FileProvider::findPathToFile("tilemap.frag"), GL_FRAGMENT_SHADER)) return false;

    m_tilemapProgram = std::make_unique<ShaderProgram>();

    if (!m_tilemapProgram->link(shaders)) 
        return false;

    if (!shaders[1].loadFromFile(FileProvider::findPathToFile("healthbar.frag"), GL_FRAGMENT_SHADER)) return false;

    m_healthbarProgram = std::make_unique<ShaderProgram>();

    if(!m_healthbarProgram->link(shaders))
        return false;

    glUseProgram(m_tilemapProgram->getHandle());
    glUniform1i(m_tilemapProgram->getUniformLocation("texture0"), 0);
    glUseProgram(0);

    glUseProgram(m_healthbarProgram->getHandle());
    glUniform1i(m_healthbarProgram->getUniformLocation("texture0"), 0);
    glUseProgram(0);

    m_tilemap = std::make_unique<TileMap>(holder);

    if(!m_tilemap->loadFromFile(FileProvider::findPathToFile("Level-1.tmx"))) 
        return false;

//  Textures
    const auto textureHandles = holder.create<Texture, 5>();

    m_textureGoomba         = std::make_unique<Texture>(textureHandles[0], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_textureMegaman        = std::make_unique<Texture>(textureHandles[1], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_textureBullet         = std::make_unique<Texture>(textureHandles[2], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_textureMovingPlatform = std::make_unique<Texture>(textureHandles[3], GL_TEXTURE_2D, 0, 0, 0, false, false);
    m_textureHealthBar      = std::make_unique<Texture>(textureHandles[4], GL_TEXTURE_2D, 0, 0, 0, false, false);

    if(!m_textureGoomba->loadFromFile(FileProvider::findPathToFile("enemy.png"), false, false))                  return false;
    if(!m_textureMegaman->loadFromFile(FileProvider::findPathToFile("megaman.png"), false, false))               return false;
    if(!m_textureBullet->loadFromFile(FileProvider::findPathToFile("bullet.png"), false, false))                 return false;
    if(!m_textureMovingPlatform->loadFromFile(FileProvider::findPathToFile("movingPlatform.png"), false, false)) return false;
    if(!m_textureHealthBar->loadFromFile(FileProvider::findPathToFile("HealthBar.png"), false, false))           return false;

    glUseProgram(m_healthbarProgram->getHandle());

    if(auto borderThickness = m_healthbarProgram->getUniformLocation("borderThickness"); borderThickness != -1)
    {
        const vec2 ratio = { 1.f / m_textureHealthBar->width, 1.f / m_textureHealthBar->height };

        float left   = 3 * ratio[0];
        float top    = 3 * ratio[1];
        float right  = 10 * ratio[0];
        float bottom = 66 * ratio[1];
        
        glUniform4f(borderThickness, left, top, right, bottom);
    }

    if(auto hpUniform = m_healthbarProgram->getUniformLocation("hp"); hpUniform != -1)
        glUniform1i(hpUniform, 100);
    
    glUseProgram(0);

//  Sprite animation
    m_sprites = std::make_unique<SpriteHolder>(buffers[1]);

    const auto vaoHandles = holder.create<VertexArrayObject, 1>();
    m_vao = std::make_unique<VertexArrayObject>(vaoHandles[0]);

    const std::array<VertexBufferLayout::Attribute, 1> spriteAttributes
    {
        VertexBufferLayout::Attribute::Float4
    };

    m_vao->addVertexBuffer(m_sprites->getVertexBuffer(), spriteAttributes);

//  Goomba
    {
        std::array<ivec4s, 2> goombaWalkFrames; 
        goombaWalkFrames[0] = { 0, 0, 16, 16  };
        goombaWalkFrames[1] = { 16, 0, 16, 16 };
        
        m_sprites->createCustomAnimaton(GOOMBA_WALK, m_textureGoomba.get(), goombaWalkFrames);
        m_sprites->createSingleAnimation(GOOMBA_DEAD, m_textureGoomba.get(), { 48, 0, 16, 16 });
    }

//  Bullet
    {
        std::array<ivec4s, 3> bulletExplodeFrames;       
        bulletExplodeFrames[0] = { 32, 9, 7,  8  };
        bulletExplodeFrames[1] = { 58, 6, 13, 13 };
        bulletExplodeFrames[2] = { 85, 5, 16, 16 };
        
        m_sprites->createSingleAnimation(BULLET_MOVE, m_textureBullet.get(), { 8, 10, 6, 6 });
        m_sprites->createCustomAnimaton(BULLET_EXPLODE, m_textureBullet.get(), bulletExplodeFrames);
    }

// Moving platform
    m_sprites->createSingleAnimation(PLATFORM_MOVE, m_textureMovingPlatform.get(), { 0, 0, 95, 22 });

//  Health bar
    ivec4s healthBarFrame = { 0, 0, m_textureHealthBar->width, m_textureHealthBar->height };
    m_sprites->createSingleAnimation(HEALTH_BAR, m_textureHealthBar.get(), healthBarFrame);
    
//  Megaman
    m_sprites->loadSpriteSheet(FileProvider::findPathToFile("anim_megaman.xml"), m_textureMegaman.get());

    m_goomba = std::make_unique<Animator>();
    {
        auto goombaWalkAnim = m_sprites->getSprites(GOOMBA_WALK);
        auto goombaDeadAnim = m_sprites->getSprites(GOOMBA_DEAD);
        m_goomba->addAnimation(GOOMBA_WALK, goombaWalkAnim);
        m_goomba->addAnimation(GOOMBA_DEAD, goombaDeadAnim);
        m_goomba->setLoop(true);
        m_goomba->setRate(4);
        m_goomba->play();
    }

    m_bullet = std::make_unique<Animator>();
    {
        auto bullet_move = m_sprites->getSprites(BULLET_MOVE);
        auto bullet_explode = m_sprites->getSprites(BULLET_EXPLODE);
        m_bullet->addAnimation(BULLET_MOVE, bullet_move);
        m_bullet->addAnimation(BULLET_EXPLODE, bullet_explode);
        m_bullet->setLoop(true);
        m_bullet->setRate(6);
        m_bullet->play();
    }

    m_movingPlatrorm = std::make_unique<Animator>();
    {
        auto platform_move = m_sprites->getSprites(PLATFORM_MOVE);
        m_movingPlatrorm->addAnimation(PLATFORM_MOVE, platform_move);
        m_movingPlatrorm->setLoop(false);
        m_movingPlatrorm->play();
    }

    m_megaman = std::make_unique<Animator>();
    {
        auto megaman_jump         = m_sprites->getSprites(MEGAMAN_JUMP);
        auto megaman_hit          = m_sprites->getSprites(MEGAMAN_HIT);
        auto megaman_duck         = m_sprites->getSprites(MEGAMAN_DUCK);
        auto megaman_shoot        = m_sprites->getSprites(MEGAMAN_SHOOT);
        auto megaman_shootAndWalk = m_sprites->getSprites(MEGAMAN_SHOOTANDWALK);
        auto megaman_climb        = m_sprites->getSprites(MEGAMAN_CLIMB);
        auto megaman_stay         = m_sprites->getSprites(MEGAMAN_STAY);
        auto megaman_walk         = m_sprites->getSprites(MEGAMAN_WALK);

        m_megaman->addAnimation(MEGAMAN_JUMP, megaman_jump);
        m_megaman->addAnimation(MEGAMAN_HIT, megaman_hit);
        m_megaman->addAnimation(MEGAMAN_DUCK, megaman_duck);
        m_megaman->addAnimation(MEGAMAN_SHOOT, megaman_shoot);
        m_megaman->addAnimation(MEGAMAN_SHOOTANDWALK, megaman_shootAndWalk);
        m_megaman->addAnimation(MEGAMAN_CLIMB, megaman_climb);
        m_megaman->addAnimation(MEGAMAN_STAY, megaman_stay);
        m_megaman->addAnimation(MEGAMAN_WALK, megaman_walk);

        m_megaman->setLoop(true);
        m_megaman->setRate(12);
        m_megaman->play();
    }

    auto healthBarFrames = m_sprites->getSprites(HEALTH_BAR);
    m_healthbar = std::make_unique<Sprite2D>();
    *m_healthbar = healthBarFrames[0];

    auto enemyObjects = m_tilemap->getObjectsByName("enemy");
    auto solidObjects = m_tilemap->getObjectsByName("solid");
    auto mpObjects    = m_tilemap->getObjectsByName("MovingPlatform");
    auto allObjects   = m_tilemap->getAllObjects();

    for (const auto& object : enemyObjects)
        auto entity = m_entities.emplace_back(std::make_unique<Goomba>(*m_goomba, (int)object.bounds.position.x, (int)object.bounds.position.y)).get();

    for (const auto& object : mpObjects)
        auto entity = m_entities.emplace_back(std::make_unique<MovingPlatform>(*m_movingPlatrorm, (int)object.bounds.position.x, (int)object.bounds.position.y)).get();

    auto playerObject = m_tilemap->getObject("player");
    m_player = std::make_unique<Player>(*m_megaman, allObjects, playerObject->bounds.position.x, playerObject->bounds.position.y);

    return true;
}


void PlatformerDemo::update(const sf::Time& elapsed) noexcept
{
    static int cooldown = 0;
    static std::vector<TileMap::Object> solidObjects; 
    
    if(solidObjects.empty()) solidObjects = m_tilemap->getObjectsByName("solid");

    auto dt = elapsed.asSeconds();
    ++cooldown;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        m_player->key.Up = true;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        m_player->key.Left = true;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        m_player->key.Down = true;  

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        m_player->key.Right = true;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    {
        m_player->key.Space = true;

        if (cooldown > 10)
        {
            bool MarioLooksToTheRight = m_player->looksToTheRight;
            int posX = MarioLooksToTheRight ? static_cast<int>(m_player->hitbox.position.x - 18) : static_cast<int>(m_player->hitbox.position.x + 18);
            int posY = static_cast<int>(m_player->hitbox.position.y + 18);
            m_entities.emplace_back(std::make_unique<Bullet>(*m_bullet, solidObjects, posX, posY, MarioLooksToTheRight));
            cooldown = 0;
        }
    }

    auto quick_remove = [](std::vector<std::unique_ptr<Entity>>& v, size_t i)
    {
        v[i].swap(v.back());
        v.pop_back();
    };
    
    for (auto i = 0; i < m_entities.size(); ++i)
    {
        m_entities[i]->update(dt);

        if (!m_entities[i]->isAlive)
        {
            quick_remove(m_entities, i);
        }
    }

    m_player->update(dt);

    for (auto& it : m_entities)
    {
        if(it->is<Goomba>())
        {
            Entity* enemy = it.get();

            if (enemy->Health < 1)
                continue;

            if (m_player->hitbox.findIntersection(enemy->hitbox))
                if (m_player->dy > 0)
                {
                    enemy->dx = 0;
                    m_player->dy = -0.2;
                    enemy->Health = 0;
                }
                else if (!m_player->hit)
                {
                    m_player->Health -= 5;
                    m_player->hit = true;
                    if (m_player->looksToTheRight)
                        m_player->hitbox.position.x += 10;
                    else
                        m_player->hitbox.position.x -= 10;
                }

            for (auto& it2 : m_entities)
            {
                if(it2->is<Bullet>())
                    if (it2->Health > 0)
                        if (it2->hitbox.findIntersection(enemy->hitbox))
                        {
                            it2->Health = 0;
                            enemy->Health -= 5;
                        }
            }
        }

        if(it->is<MovingPlatform>())
        {
            if (m_player->hitbox.findIntersection(it->hitbox))
                if (m_player->dy > 0)
                {
                    if (m_player->hitbox.position.y + m_player->hitbox.size.y < it->hitbox.position.y + it->hitbox.size.y)
                    {
                        m_player->hitbox.position.y = it->hitbox.position.y - m_player->hitbox.size.y + 3;
                        m_player->hitbox.position.x += it->dx * dt;
                        m_player->dy = 0;
                        m_player->state = Player::stay;
                    }
                }
        }
    }
}


void PlatformerDemo::draw() noexcept
{
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto [width, height] = m_window.getSize();

    mat4 mvp;
    mat4 modelMatrix;
    
    vec2 playerPos;
    m_player->getPosition(playerPos);

    m_camera->setPosition(-(playerPos[0] - (width >> 1)), -(playerPos[1] - (height >> 1)));
    m_camera->getModelViewProjectionMatrix(mvp);
    m_uniformBuffer->update(0, sizeof(mat4), 1, static_cast<const void*>(mvp));

    glUseProgram(m_tilemapProgram->getHandle());

    m_tilemap->draw();

    glBindVertexArray(m_vao->getHandle());

    for(auto& entity : m_entities)
    { 
        entity->getMatrix(modelMatrix);
        glmc_mat4_mul(mvp, modelMatrix, modelMatrix);
        m_uniformBuffer->update(0, sizeof(mat4), 1, static_cast<const void*>(modelMatrix));
        entity->anim.draw();
    }

    m_player->getMatrix(modelMatrix);
    glmc_mat4_mul(mvp, modelMatrix, modelMatrix);
    m_uniformBuffer->update(0, sizeof(mat4), 1, static_cast<const void*>(modelMatrix));
    m_player->anim.draw();

    glUseProgram(0);

//  Health bar        
    glUseProgram(m_healthbarProgram->getHandle());

    if(auto hpUniform = m_healthbarProgram->getUniformLocation("hp"); hpUniform != -1)
        glUniform1i(hpUniform, m_player->Health);

    m_camera->setPosition(20, 20);
    m_camera->getModelViewProjectionMatrix(mvp);
    m_uniformBuffer->update(0, sizeof(mat4), 1, static_cast<const void*>(mvp));
    
    glBindTexture(GL_TEXTURE_2D, m_healthbar->texture);
    glDrawArrays(GL_TRIANGLE_FAN, m_healthbar->frame, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
        
    glUseProgram(0);
    glBindVertexArray(0);
}