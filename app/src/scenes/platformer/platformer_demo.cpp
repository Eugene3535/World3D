#include <array>
#include <memory>

#include <glad/glad.h>

#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/Image.hpp"
#include "files/FileProvider.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "camera/orthogonal/Orthogonal.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
#include "scenes/platformer/tilemap/TileMap.hpp"
#include "scenes/platformer/sprites/SpriteHolder.hpp"
#include "scenes/platformer/entities/Goomba.hpp"
#include "scenes/platformer/entities/Bullet.hpp"
#include "scenes/platformer/entities/MovingPlatform.hpp"
#include "scenes/platformer/entities/Player.hpp"


#define HEALTH_BAR "health_bar"

int platformer_demo(sf::Window& window)
{
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    auto [width, height] = window.getSize();

    auto resourceHolder = std::make_unique<GlResourceHolder>();
    const auto buffers = resourceHolder->create<GlBuffer, 2>();

    GlBuffer uniformBuffer(buffers[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = std::make_unique<Orthogonal>(); 
    camera->setupProjectionMatrix(width, height);

    std::array<Shader, 2> shaders;
    if (!shaders[0].loadFromFile(FileProvider::findPathToFile("tilemap.vert"), GL_VERTEX_SHADER)) return -1;
    if (!shaders[1].loadFromFile(FileProvider::findPathToFile("tilemap.frag"), GL_FRAGMENT_SHADER)) return -1;

    auto tilemapProgram = std::make_unique<ShaderProgram>();
    if (!tilemapProgram->link(shaders)) return -1;

    glUseProgram(tilemapProgram->getHandle().value());
    glUniform1i(tilemapProgram->getUniformLocation("texture0").value(), 0);
    glUseProgram(0);

    TileMap tilemap(*resourceHolder);

    if(!tilemap.loadFromFile(FileProvider::findPathToFile("Level-1.tmx"))) 
        return -1;

//  Textures
    const auto textureHandles = resourceHolder->create<Texture2D, 5>();

    auto texGoomba     = std::make_unique<Texture2D>(textureHandles[0]);
    auto texMegaman    = std::make_unique<Texture2D>(textureHandles[1]);
    auto texBullet     = std::make_unique<Texture2D>(textureHandles[2]);
    auto texMovingPlat = std::make_unique<Texture2D>(textureHandles[3]);
    auto texHealthBar  = std::make_unique<Texture2D>(textureHandles[4]);

    if(!texGoomba->loadFromFile(FileProvider::findPathToFile("enemy.png"), false, false))              return -1;
    if(!texMegaman->loadFromFile(FileProvider::findPathToFile("megaman.png"), false, false))           return -1;
    if(!texBullet->loadFromFile(FileProvider::findPathToFile("bullet.png"), false, false))             return -1;
    if(!texMovingPlat->loadFromFile(FileProvider::findPathToFile("movingPlatform.png"), false, false)) return -1;
    if(!texHealthBar->loadFromFile(FileProvider::findPathToFile("HealthBar.png"), false, false))       return -1;

//  Sprite animation
    SpriteHolder spriteHolder(buffers[1]);

    const auto vaoHandles = resourceHolder->create<VertexArrayObject, 1>();
    auto vao = std::make_unique<VertexArrayObject>(vaoHandles[0]);

    const std::array<VertexBufferLayout::Attribute, 1> spriteAttributes
    {
        VertexBufferLayout::Attribute::Float4
    };
    const VertexBufferLayout spriteLayout(spriteAttributes);
    vao->addVertexBuffer(spriteHolder.getVertexBuffer(), spriteLayout);

//  Goomba
    {
        std::array<glm::ivec4, 2> goombaWalkFrames = 
        {
            glm::ivec4(0, 0, 16, 16),
            glm::ivec4(16, 0, 16, 16)
        };
        spriteHolder.createCustomAnimaton(GOOMBA_WALK, texGoomba.get(), goombaWalkFrames);
        spriteHolder.createSingleAnimation(GOOMBA_DEAD, texGoomba.get(), glm::ivec4(48, 0, 16, 16));
    }

//  Bullet
    {
        std::array<glm::ivec4, 3> bulletExplodeFrames = 
        {
            glm::ivec4(32, 9, 7, 8),
            glm::ivec4(58, 6, 13, 13),
            glm::ivec4(85, 5, 16, 16)
        };
        spriteHolder.createSingleAnimation(BULLET_MOVE, texBullet.get(), glm::ivec4(8, 10, 6, 6));
        spriteHolder.createCustomAnimaton(BULLET_EXPLODE, texBullet.get(), bulletExplodeFrames);
    }

// Moving platform
    spriteHolder.createSingleAnimation(PLATFORM_MOVE, texMovingPlat.get(), glm::ivec4(0, 0, 95, 22));

//  Health bar
    spriteHolder.createSingleAnimation(HEALTH_BAR, texHealthBar.get(), glm::ivec4(0, 0, texHealthBar->getWidth(), texHealthBar->getHeight()));
    
//  Megaman
    spriteHolder.loadSpriteSheet(FileProvider::findPathToFile("anim_megaman.xml"), texMegaman.get());

    Animator goomba;
    {
        auto goombaWalkAnim = spriteHolder.getSprites(GOOMBA_WALK);
        auto goombaDeadAnim = spriteHolder.getSprites(GOOMBA_DEAD);
        goomba.addAnimation(GOOMBA_WALK, goombaWalkAnim);
        goomba.addAnimation(GOOMBA_DEAD, goombaDeadAnim);
        goomba.setLoop(true);
        goomba.setRate(4);
        goomba.play();
    }

    Animator bullet;
    {
        auto bullet_move = spriteHolder.getSprites(BULLET_MOVE);
        auto bullet_explode = spriteHolder.getSprites(BULLET_EXPLODE);
        bullet.addAnimation(BULLET_MOVE, bullet_move);
        bullet.addAnimation(BULLET_EXPLODE, bullet_explode);
        bullet.setLoop(true);
        bullet.setRate(6);
        bullet.play();
    }

    Animator movingPlatrorm;
    {
        auto platform_move = spriteHolder.getSprites(PLATFORM_MOVE);
        movingPlatrorm.addAnimation(PLATFORM_MOVE, platform_move);
        movingPlatrorm.setLoop(false);
        movingPlatrorm.play();
    }

    Animator megaman;
    {
        auto megaman_jump         = spriteHolder.getSprites(MEGAMAN_JUMP);
        auto megaman_hit          = spriteHolder.getSprites(MEGAMAN_HIT);
        auto megaman_duck         = spriteHolder.getSprites(MEGAMAN_DUCK);
        auto megaman_shoot        = spriteHolder.getSprites(MEGAMAN_SHOOT);
        auto megaman_shootAndWalk = spriteHolder.getSprites(MEGAMAN_SHOOTANDWALK);
        auto megaman_climb        = spriteHolder.getSprites(MEGAMAN_CLIMB);
        auto megaman_stay         = spriteHolder.getSprites(MEGAMAN_STAY);
        auto megaman_walk         = spriteHolder.getSprites(MEGAMAN_WALK);

        megaman.addAnimation(MEGAMAN_JUMP, megaman_jump);
        megaman.addAnimation(MEGAMAN_HIT, megaman_hit);
        megaman.addAnimation(MEGAMAN_DUCK, megaman_duck);
        megaman.addAnimation(MEGAMAN_SHOOT, megaman_shoot);
        megaman.addAnimation(MEGAMAN_SHOOTANDWALK, megaman_shootAndWalk);
        megaman.addAnimation(MEGAMAN_CLIMB, megaman_climb);
        megaman.addAnimation(MEGAMAN_STAY, megaman_stay);
        megaman.addAnimation(MEGAMAN_WALK, megaman_walk);

        megaman.setLoop(true);
        megaman.setRate(12);
        megaman.play();
    }

    auto healthBarFrame = spriteHolder.getSprites(HEALTH_BAR);
    const auto healthbar = healthBarFrame.data();

    auto enemyObjects = tilemap.getObjectsByName("enemy");
    auto solidObjects = tilemap.getObjectsByName("solid");
    auto mpObjects    = tilemap.getObjectsByName("MovingPlatform");
    auto allObjects   = tilemap.getAllObjects();

    std::vector<std::unique_ptr<Entity>> entities;

    for (const auto& object : enemyObjects)
        auto entity = entities.emplace_back(std::make_unique<Goomba>(goomba, (int)object.bounds.left, (int)object.bounds.top)).get();

    for (const auto& object : mpObjects)
        auto entity = entities.emplace_back(std::make_unique<MovingPlatform>(movingPlatrorm, (int)object.bounds.left, (int)object.bounds.top)).get();

    auto playerObject = tilemap.getObject("player");
    Player Mario(megaman, allObjects, playerObject->bounds.left, playerObject->bounds.top);
        
    sf::Clock clock;

    int cooldown = 0;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::Resized)
            {
                width = event.size.width;
                height = event.size.height;
                camera->setupProjectionMatrix(width, height);
                glViewport(0, 0, width, height);
            }
        }

        auto dt = clock.restart().asSeconds();
        ++cooldown;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            window.close();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            Mario.key.Up = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            Mario.key.Left = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            Mario.key.Down = true;  

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            Mario.key.Right = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        {
            Mario.key.Space = true;

            if (cooldown > 10)
            {
                bool MarioLooksToTheRight = Mario.looksToTheRight;
                int posX = MarioLooksToTheRight ? static_cast<int>(Mario.hitbox.left - 18) : static_cast<int>(Mario.hitbox.left + 18);
                int posY = static_cast<int>(Mario.hitbox.top + 18);
                entities.emplace_back(std::make_unique<Bullet>(bullet, solidObjects, posX, posY, MarioLooksToTheRight));
                cooldown = 0;
            }
        }

        auto quick_remove = [](std::vector<std::unique_ptr<Entity>>& v, size_t i)
		{
			v[i].swap(v.back());
			v.pop_back();
		};
        
        for (auto i = 0; i < entities.size(); ++i)
		{
			entities[i]->update(dt);

			if (!entities[i]->isAlive)
			{
				quick_remove(entities, i);
			}
		}

        Mario.update(dt);

		for (auto& it : entities)
		{
			if(it->is<Goomba>())
			{
				Entity* enemy = it.get();

				if (enemy->Health < 1)
					continue;

				if (Mario.hitbox.intersects(enemy->hitbox))
					if (Mario.dy > 0)
					{
						enemy->dx = 0;
						Mario.dy = -0.2;
						enemy->Health = 0;
					}
					else if (!Mario.hit)
					{
						Mario.Health -= 5;
						Mario.hit = true;
						if (Mario.looksToTheRight)
							Mario.hitbox.left += 10;
						else
							Mario.hitbox.left -= 10;
					}

				for (auto& it2 : entities)
				{
					if(it2->is<Bullet>())
						if (it2->Health > 0)
							if (it2->hitbox.intersects(enemy->hitbox))
							{
								it2->Health = 0;
								enemy->Health -= 5;
							}
				}
			}

			if(it->is<MovingPlatform>())
			{
				if (Mario.hitbox.intersects(it->hitbox))
					if (Mario.dy > 0)
					{
						if (Mario.hitbox.top + Mario.hitbox.height < it->hitbox.top + it->hitbox.height)
						{
							Mario.hitbox.top = it->hitbox.top - Mario.hitbox.height + 3;
							Mario.hitbox.left += it->dx * dt;
							Mario.dy = 0;
							Mario.state = Player::stay;
						}
					}
			}
		}

        camera->setPosition(-(Mario.getPosition().x - (width >> 1)), -(Mario.getPosition().y - (height >> 1)));
        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glUseProgram(tilemapProgram->getHandle().value());

        glClear(GL_COLOR_BUFFER_BIT);
        tilemap.draw();

        glBindVertexArray(vao->getHandle());

        for(auto& entity : entities)
        {
            uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix() * entity->getMatrix())));
            entity->anim.draw();
        }

        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix() * Mario.getMatrix())));
        Mario.anim.draw();

        camera->setPosition(20, 20);
        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));
        glBindTexture(GL_TEXTURE_2D, healthbar->texture);
		glDrawArrays(GL_TRIANGLE_FAN, healthbar->frame, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
            
        glBindVertexArray(0);
        glUseProgram(0);

        window.display();
    }

    return 0;
}