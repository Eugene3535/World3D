#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include <glm/glm.hpp>

#include "opengl/holder/GlResourceHolder.hpp"

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
		Texture2D texture;
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

		glm::ivec2 position;
		glm::ivec2 size;
	};

	TileMap(GlResourceHolder& rh) noexcept;
	~TileMap() noexcept;

	bool loadFromFile(const std::filesystem::path& filepath) noexcept;

private:
	bool loadTilePlanes(const void* node) noexcept;
	bool loadObjects(const void* node)    noexcept;
	std::vector<Tileset> loadTilesets(const void* node)   noexcept;
	std::vector<int32_t> parseCSVstring(const void* node) noexcept;

	GlResourceHolder& m_resourceHolder;
    
	std::unique_ptr<Plane> m_background;
	std::unique_ptr<Plane> m_foreground;

    std::vector<Object> m_objects;
    glm::uvec2          m_mapSize;
    glm::uvec2          m_tileSize;
};

#endif // !TILEMAP_HPP