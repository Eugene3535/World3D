#include <array>
#include <algorithm>
#include <cstring>

#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_utils.hpp"

#include "files/StbImage.hpp"
#include "files/FileProvider.hpp"
#include "scenes/platformer/tilemap/TileMap.hpp"


TileMap::Plane::Plane(const GLuint vboHandle, const GLuint eboHandle, const GLuint vaoHandle, const GLuint textureHandle) noexcept:
    vbo(vboHandle, GL_ARRAY_BUFFER),
    ebo(eboHandle, GL_ELEMENT_ARRAY_BUFFER),
    vao(vaoHandle),
    texture(textureHandle)
{

}


TileMap::TileMap(GlResourceHolder& rh) noexcept:
    m_resourceHolder(rh),
    m_mapSize(),
    m_tileSize()
{
    const auto buffers = rh.create<GlBuffer, 4>();
    const auto vertexArrays = rh.create<VertexArrayObject, 2>();
    const auto textures = rh.create<Texture, 2>();

    m_background = std::make_unique<TileMap::Plane>(buffers[0], buffers[1], vertexArrays[0], textures[0]);
    m_foreground = std::make_unique<TileMap::Plane>(buffers[2], buffers[3], vertexArrays[1], textures[1]);
}


TileMap::~TileMap() noexcept
{
    std::array<GLuint, 4> buffers = {};
    std::array<GLuint, 2> vertexArrays = {};
    std::array<GLuint, 2> textures = {};

    buffers[0] = m_background->vbo.getHandle();
    buffers[1] = m_foreground->vbo.getHandle();
    buffers[2] = m_background->ebo.getHandle();
    buffers[3] = m_foreground->ebo.getHandle();

    vertexArrays[0] = m_background->vao.getHandle();
    vertexArrays[1] = m_foreground->vao.getHandle();

    textures[0] = m_background->texture.handle;
    textures[1] = m_foreground->texture.handle;

    m_resourceHolder.destroy<GlBuffer>(buffers);
    m_resourceHolder.destroy<VertexArrayObject>(vertexArrays);
    m_resourceHolder.destroy<Texture>(textures);
}


bool TileMap::loadFromFile(const std::filesystem::path& filepath) noexcept
{
    auto document = std::make_unique<rapidxml::xml_document<char>>();
    rapidxml::file<char> xmlFile(filepath.string().c_str());
    document->parse<0>(xmlFile.data());
    const auto mapNode = document->first_node("map");

    if (loadTilePlanes(static_cast<const void*>(mapNode)))
        if (loadObjects(static_cast<const void*>(mapNode)))
            return true;

    return false;
}


void TileMap::draw() noexcept
{
    glBindTexture(GL_TEXTURE_2D, m_background->texture.handle);
    glBindVertexArray(m_background->vao.getHandle());
    glDrawElements(GL_TRIANGLES, m_background->ebo.getCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, m_foreground->texture.handle);
    glBindVertexArray(m_foreground->vao.getHandle());
    glDrawElements(GL_TRIANGLES, m_foreground->ebo.getCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


const TileMap::Object* TileMap::getObject(const std::string& name) const noexcept
{
    for (const auto& object : m_objects)
        if(object.name == name)
            return &object;
    
    return nullptr;
}


std::vector<TileMap::Object> TileMap::getObjectsByName(const std::string& name) const noexcept
{
    std::vector<TileMap::Object> objectsByName;

    for (const auto& object : m_objects)
        if(object.name == name)
            objectsByName.push_back(object);

    return objectsByName;
}


std::vector<TileMap::Object> TileMap::getObjectsByType(const std::string& type) const noexcept
{
    std::vector<TileMap::Object> objectsByType;

    for (const auto& object : m_objects)
        if(object.type == type)
            objectsByType.push_back(object);

    return objectsByType;
}


std::span<const TileMap::Object> TileMap::getAllObjects() const noexcept
{
    return m_objects;
}


bool TileMap::loadTilePlanes(const void* node) noexcept
{
	const auto mapNode = static_cast<const rapidxml::xml_node<char>*>(node);

	auto tilesets = loadTilesets(mapNode);

	if (tilesets.empty())
		return false;

	auto mapW  = mapNode->first_attribute("width");
	auto mapH  = mapNode->first_attribute("height");
	auto tileW = mapNode->first_attribute("tilewidth");
	auto tileH = mapNode->first_attribute("tileheight");

	const int32_t map_width   = mapW  ? std::atoi(mapW->value())  : 0;
	const int32_t map_height  = mapH  ? std::atoi(mapH->value())  : 0;
	const int32_t tile_width  = tileW ? std::atoi(tileW->value()) : 0;
	const int32_t tile_height = tileH ? std::atoi(tileH->value()) : 0;

	m_mapSize  = { map_width,  map_height  };
	m_tileSize = { tile_width, tile_height };

	bool isBackground = true; // first plane is background, second is foreground

	for (auto layerNode = mapNode->first_node("layer");
		      layerNode != nullptr;
		      layerNode = layerNode->next_sibling("layer"))
	{
		auto dataNode = layerNode->first_node("data");
		auto parsed_layer = parseCSVstring(dataNode);

		Plane* plane = isBackground ? m_background.get() : m_foreground.get();
        const Tileset* tileset = tilesets.data();

        if (!isBackground)
            if (tilesets.size() > 1)
                tileset++;

		std::vector<GLfloat> vertices;
		std::vector<GLuint>  indices;

		sf::Vector2f ratio = sf::Vector2f(1.0f / (float)plane->texture.width, 1.0f / (float)plane->texture.height);
        size_t index = 0;
        isBackground = false;

        for (int32_t y = 0u; y < map_height; ++y)
        {
            for (int32_t x = 0u; x < map_width; ++x)
            {
                int32_t tileId = parsed_layer[index];
                ++index;

                if (tileId)
                {
//                  positions
                    sf::Vector2f leftBottom  = sf::Vector2f(x * tile_width,              y * tile_height + tile_height);
                    sf::Vector2f rightBottom = sf::Vector2f(x * tile_width + tile_width, y * tile_height + tile_height);
                    sf::Vector2f rightTop    = sf::Vector2f(x * tile_width + tile_width, y * tile_height);
                    sf::Vector2f leftTop     = sf::Vector2f(x * tile_width,              y * tile_height);

//                  tex coords
                    int32_t tileNum = tileId - tileset->firstGID;

                    int32_t vY = (tileNum >= tileset->columns) ? tileNum / tileset->columns : 0;
                    int32_t vX = tileNum % tileset->columns;

                    int32_t offsetX = vX * tile_width;
                    int32_t offsetY = vY * tile_height;

                    float left   = offsetX * ratio.x;
                    float top    = offsetY * ratio.y;
                    float right  = (offsetX + tile_width) * ratio.x;
                    float bottom = (offsetY + tile_height) * ratio.y;

                    const GLuint cell = static_cast<GLuint>(vertices.size() >> 2);

                    vertices.push_back(leftBottom.x);
                    vertices.push_back(leftBottom.y);
                    vertices.push_back(left);
                    vertices.push_back(bottom);

                    vertices.push_back(rightBottom.x);
                    vertices.push_back(rightBottom.y);
                    vertices.push_back(right);
                    vertices.push_back(bottom);

                    vertices.push_back(rightTop.x);
                    vertices.push_back(rightTop.y);
                    vertices.push_back(right);
                    vertices.push_back(top);

                    vertices.push_back(leftTop.x);
                    vertices.push_back(leftTop.y);
                    vertices.push_back(left);
                    vertices.push_back(top);

                    indices.push_back(cell);
                    indices.push_back(cell + 1u);
                    indices.push_back(cell + 2u);

                    indices.push_back(cell);
                    indices.push_back(cell + 2u);
                    indices.push_back(cell + 3u);
                }
            }
        }

        plane->vbo.create(sizeof(GLfloat), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);
        plane->ebo.create(sizeof(GLuint), indices.size(), static_cast<const void*>(indices.data()), GL_STATIC_DRAW);

        const std::array<VertexBufferLayout::Attribute, 1> attributes =
        {
            VertexBufferLayout::Attribute::Float4
        };

        plane->vao.addVertexBuffer(plane->vbo, attributes);
        plane->vao.setElementBuffer(plane->ebo);
	}

	return true;
}


bool TileMap::loadObjects(const void* node) noexcept
{
    const auto mapNode = static_cast<const rapidxml::xml_node<char>*>(node);

    for (auto objectGroupNode = mapNode->first_node("objectgroup");
              objectGroupNode != nullptr;
              objectGroupNode = objectGroupNode->next_sibling("objectgroup"))
    {
        for (auto objectNode = objectGroupNode->first_node("object");
                  objectNode != nullptr;
                  objectNode = objectNode->next_sibling("object"))
        {
            auto& object = m_objects.emplace_back();

            for (auto attr = objectNode->first_attribute(); attr != nullptr; attr = attr->next_attribute())
            {
                if (strcmp(attr->name(), "x") == 0)      object.bounds.position.x = atof(attr->value());
                if (strcmp(attr->name(), "y") == 0)      object.bounds.position.y = atof(attr->value());
                if (strcmp(attr->name(), "width") == 0)  object.bounds.size.x     = atof(attr->value());
                if (strcmp(attr->name(), "height") == 0) object.bounds.size.y     = atof(attr->value());
                if (strcmp(attr->name(), "name") == 0)   object.name = attr->value();
                if (strcmp(attr->name(), "class") == 0)  object.type = attr->value();
            }

            if (const auto propertiesNode = objectNode->first_node("properties"); propertiesNode != nullptr)
            {
                for (auto propertyNode = propertiesNode->first_node("property");
                    propertyNode != nullptr;
                    propertyNode = propertyNode->next_sibling("property"))
                {
                    auto& prop = object.properties.emplace_back();

                    for (auto attr = propertyNode->first_attribute(); attr != nullptr; attr = attr->next_attribute())
                    {
                        if (strcmp(attr->name(), "name") == 0) { prop.name = attr->value(); continue; }
                        if (strcmp(attr->name(), "type") == 0) { prop.type = attr->value(); continue; }
                        if (strcmp(attr->name(), "value") == 0)   prop.value = attr->value();
                    }
                }
            }
        }
    }

    return !m_objects.empty();
}


std::vector<TileMap::Tileset> TileMap::loadTilesets(const void* node) noexcept
{
    StbImage tilesetImage;
    tilesetImage.loadFromFile(FileProvider::findPathToFile("tileset1.png"));

    m_background->texture.loadFromImage(tilesetImage, false, false);
    m_foreground->texture.loadFromImage(tilesetImage, false, false);

    std::vector<TileMap::Tileset> tilesets;
    const auto mapNode = static_cast<const rapidxml::xml_node<char>*>(node);

    for (auto tilesetNode = mapNode->first_node("tileset");
              tilesetNode != nullptr;
              tilesetNode = tilesetNode->next_sibling("tileset"))
    {
        Tileset& ts = tilesets.emplace_back();

        auto tileCount = tilesetNode->first_attribute("tilecount");
        auto columns   = tilesetNode->first_attribute("columns");
        auto firstGID  = tilesetNode->first_attribute("firstgid");

        ts.tileCount = tileCount ? std::atoi(tileCount->value()) : 0;
        ts.columns   = columns ? std::atoi(columns->value()) : 0;
        ts.rows      = (!tileCount || !columns) ? 0 : ts.tileCount / ts.columns;
        ts.firstGID  = firstGID ? std::atoi(firstGID->value()) : 0;
    }

    return tilesets;
}


std::vector<int32_t> TileMap::parseCSVstring(const void* node) noexcept
{
    const auto dataNode = static_cast<const rapidxml::xml_node<char>*>(node);

    std::string data(dataNode->value());
    std::replace(data.begin(), data.end(), ',', ' ');

    std::vector<int32_t> parsed;

    std::stringstream sstream(data);
    {
        int32_t tileId = 0;

        while (sstream >> tileId)
            parsed.push_back(tileId);
    }

    return parsed;
}