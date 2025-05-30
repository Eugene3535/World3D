#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include <SFML/Graphics/Rect.hpp>

#include "resources/holder/GlResourceHolder.hpp"

class TileMap
{
	class Tileset
	{
	public:
		int32_t columns = 0;
		int32_t rows = 0;
		int32_t tileCount = 0;
		int32_t firstGID = 1;
	};

public:
	class Plane
	{
    public:
		Plane(const GLuint vboHandle, const GLuint eboHandle, const GLuint vaoHandle, const GLuint textureHandle) noexcept;

		GlBuffer vbo;
		GlBuffer ebo;
		VertexArrayObject vao;
		Texture texture;
	};

	class Object
	{
    public:
		class Property
		{
        public:
			std::string name;
			std::string type;
			std::string value;
		};

		std::string name;
		std::string type;

		std::vector<Property> properties;

		sf::FloatRect bounds;
	};

	TileMap(GlResourceHolder& rh) noexcept;
	~TileMap() noexcept;

	bool loadFromFile(const std::filesystem::path& filepath) noexcept;
	void draw() noexcept;

	const Object*             getObject(const std::string& name) const noexcept;
	std::vector<Object> getObjectsByName(const std::string& name) const noexcept;
	std::vector<Object> getObjectsByType(const std::string& type) const noexcept;
	std::span<const Object>   getAllObjects() const noexcept;

private:
	bool loadTilePlanes(const void* node) noexcept;
	bool loadObjects(const void* node)    noexcept;
	std::vector<Tileset> loadTilesets(const void* node)   noexcept;
	std::vector<int32_t> parseCSVstring(const void* node) noexcept;

	GlResourceHolder& m_resourceHolder;
    
	std::unique_ptr<Plane> m_background;
	std::unique_ptr<Plane> m_foreground;

    std::vector<Object> m_objects;
    sf::Vector2i        m_mapSize;
    sf::Vector2i        m_tileSize;
};

#endif // !TILEMAP_HPP