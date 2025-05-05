#ifndef SPRITE_HOLDER_HPP
#define SPRITE_HOLDER_HPP

#include <string>
#include <utility>
#include <vector>
#include <span>
#include <unordered_map>
#include <filesystem>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include "resources/buffers/GlBuffer.hpp"
#include "scenes/platformer/sprites/Sprite2D.hpp"


class SpriteHolder final
{
	using sprite_range = std::pair<GLuint, GLuint>; // start sprite number, duration
public:
	SpriteHolder(const GLuint bufferHandle) noexcept;

	void createSingleAnimation(const std::string& name, const class Texture* texture, const glm::ivec4& frame) noexcept;
	void createLinearAnimaton(const std::string& name, const class Texture* texture, GLuint duration) noexcept;
	void createGridAnimaton(const std::string& name, const class Texture* texture, GLuint columns, GLuint rows) noexcept;
	void createCustomAnimaton(const std::string& name, const class Texture* texture, std::span<const glm::ivec4> frames) noexcept;
	void loadSpriteSheet(const std::filesystem::path& filePath, const class Texture* texture) noexcept;

	const GlBuffer& getVertexBuffer() const noexcept;
	std::span<const Sprite2D> getSprites(const std::string& name) const noexcept;

private:
	void addSprite(GLuint texture, const glm::ivec4& frame, const glm::vec2& ratio) noexcept;

	GlBuffer m_vertexBufferObject;
	std::unordered_map<std::string, sprite_range> m_animations;
	std::vector<Sprite2D> m_sprites;
	std::vector<float> m_vertices;
};

#endif // !SPRITE_HOLDER_HPP