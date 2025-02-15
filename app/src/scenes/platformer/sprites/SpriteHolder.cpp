#include <array>

#include "RapidXML/rapidxml_utils.hpp"

#include "opengl/resources/graphics/Texture2D.hpp"
#include "scenes/platformer/sprites/SpriteHolder.hpp"


SpriteHolder::SpriteHolder(const GLuint bufferHandle) noexcept:
	m_vertexBufferObject(bufferHandle, GL_ARRAY_BUFFER)
{
	m_vertexBufferObject.create(sizeof(float), 0, nullptr, GL_STATIC_DRAW);
}


void SpriteHolder::createSprite(const std::string& name, const Texture2D* texture, const glm::ivec4& frame) noexcept
{
	if(auto it = m_animations.find(name); it == m_animations.end())
	{
		const GLuint id = m_sprites.size();
		m_animations.emplace(name, sprite_range(id, 1));

		const auto ratio = 1.0f / glm::vec2(texture->getWidth(), texture->getHeight());
		addSprite(texture->getHandle(), frame, ratio);

		m_vertexBufferObject.update(0, sizeof(float), m_vertices.size(), static_cast<const void*>(m_vertices.data()));
	}
}


void SpriteHolder::createLinearAnimaton(const std::string& name, const Texture2D* texture, GLuint duration) noexcept
{
	if(auto it = m_animations.find(name); it == m_animations.end())
	{
		const GLuint id = m_sprites.size();
		m_animations.emplace(name, sprite_range(id, duration));

		const auto size  = glm::vec2(texture->getWidth(), texture->getHeight());
		const auto ratio = 1.0f / size;
		const GLuint frameWidth = texture->getWidth() / duration;
		const GLuint handle = texture->getHandle();
	
		for (GLuint i = 0; i < duration; ++i)
			addSprite(handle, glm::ivec4(i * frameWidth, 0, frameWidth, size.y), ratio);

		m_vertexBufferObject.update(0, sizeof(float), m_vertices.size(), static_cast<const void*>(m_vertices.data()));
	}
}


void SpriteHolder::createGridAnimaton(const std::string& name, const Texture2D* texture, GLuint columns, GLuint rows) noexcept
{
	if(auto it = m_animations.find(name); it == m_animations.end())
	{
		const GLuint id = m_sprites.size();
		const GLuint duration = columns * rows;
		m_animations.emplace(name, sprite_range(id, duration));

		const auto size  = glm::vec2(texture->getWidth(), texture->getHeight());
		const auto ratio = 1.0f / size;

		const int32_t width  = size.x / static_cast<float>(columns);
		const int32_t height = size.y / static_cast<float>(rows);
		const GLuint handle = texture->getHandle();
	
		for (GLuint y = 0; y < rows; ++y)
			for (GLuint x = 0; x < columns; ++x)	
				addSprite(handle, glm::ivec4(x * width, y * height, width, height), ratio);

		m_vertexBufferObject.update(0, sizeof(float), m_vertices.size(), static_cast<const void*>(m_vertices.data()));
	}
}


void SpriteHolder::loadSpriteSheet(const std::filesystem::path& filePath, const Texture2D* texture) noexcept
{
	auto document = std::make_unique<rapidxml::xml_document<char>>();
	rapidxml::file<char> xmlFile(filePath.string().c_str());
	document->parse<0>(xmlFile.data());
	const auto spriteNode = document->first_node("sprites");

	const auto size  = glm::vec2(texture->getWidth(), texture->getHeight());
	const auto ratio = 1.0f / size;
	const GLuint handle = texture->getHandle();

	for(auto animNode = spriteNode->first_node("animation");
		     animNode != nullptr;
		     animNode = animNode->next_sibling("animation"))
	{
		const std::string title = animNode->first_attribute("title")->value();
		const GLuint id = m_sprites.size();

		std::vector<glm::ivec4> frames;

		if(auto it = m_animations.find(title); it == m_animations.end())
		{
			auto cutNode = animNode->first_node("cut");

			while (cutNode)
			{
				auto pX = cutNode->first_attribute("x");
				auto pY = cutNode->first_attribute("y");
				auto pW = cutNode->first_attribute("w");
				auto pH = cutNode->first_attribute("h");

				int x = pX ? atoi(pX->value()) : 0;
				int y = pY ? atoi(pY->value()) : 0;
				int w = pW ? atoi(pW->value()) : 0;
				int h = pH ? atoi(pH->value()) : 0;

				frames.emplace_back(x, y, w, h);

				cutNode = cutNode->next_sibling();
			}
		}

		if(!frames.empty())
		{
			m_animations.emplace(title, sprite_range(id, static_cast<GLuint>(frames.size())));

			for(const auto& frame : frames)
				addSprite(handle, frame, ratio);
		}
	}

	m_vertexBufferObject.update(0, sizeof(float), m_vertices.size(), static_cast<const void*>(m_vertices.data()));
}


const GlBuffer& SpriteHolder::getVertexBuffer() const noexcept
{
	return m_vertexBufferObject;
}


std::span<const Sprite2D> SpriteHolder::getSprites(const std::string& name) const noexcept
{
	if(auto it = m_animations.find(name); it != m_animations.end())
	{
		const auto sprites = m_sprites.data();
		size_t startFrame = static_cast<size_t>(it->second.first);
		size_t duration = static_cast<size_t>(it->second.second);

		return std::span<const Sprite2D>(sprites + startFrame, duration);
	}

	return std::span<const Sprite2D>();
}


void SpriteHolder::addSprite(GLuint texture, const glm::ivec4& frame, const glm::vec2& ratio) noexcept
{
	GLuint id  = m_sprites.size();
	auto& sprite = m_sprites.emplace_back();

	sprite.frame   = id;
	sprite.texture = texture;
	sprite.width   = frame.z;
	sprite.height  = frame.w;

	std::array<float, 16> spriteVertices = {};
	float* quad = spriteVertices.data();

	quad[4]  = static_cast<float>(frame.z);
	quad[8]  = static_cast<float>(frame.z);
	quad[9]  = static_cast<float>(frame.w);
	quad[13] = static_cast<float>(frame.w);

	float left   = frame.x * ratio.x;
	float top    = frame.y * ratio.y;
	float right  = (frame.x + frame.z) * ratio.x;
	float bottom = (frame.y + frame.w) * ratio.y;

	quad[2] = left;
	quad[3] = top;

	quad[6] = right;
	quad[7] = top;

	quad[10] = right;
	quad[11] = bottom;

	quad[14] = left;
	quad[15] = bottom;

	m_vertices.insert(m_vertices.end(), spriteVertices.begin(), spriteVertices.end());
}