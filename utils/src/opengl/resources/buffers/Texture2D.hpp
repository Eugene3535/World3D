#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <filesystem>

#include <glad/glad.h>

class Texture2D final
{
public:
	Texture2D(GLuint handle) noexcept;

	bool loadFromImage(const class Image& image, bool repeat, bool smooth) noexcept;
	bool loadFromFile(const std::filesystem::path& filePath, bool repeat, bool smooth) noexcept;

	bool create(GLuint width, GLuint height) noexcept;
	void update(GLuint x, GLuint y, GLuint width, GLuint height, const uint8_t* pixels) noexcept;

	bool copyToImage(Image& image) noexcept;

	GLuint getHandle() const noexcept;
	GLuint getWidth()  const noexcept;
	GLuint getHeight() const noexcept;

	void setSmooth(bool smooth)    noexcept;
	void setRepeated(bool repeate) noexcept;

	bool isSmooth()   const noexcept;
	bool isRepeated() const noexcept;

	static GLuint getMaximumSize() noexcept;

private:
	GLuint m_handle;
	GLuint m_width;
    GLuint m_height;
	bool   m_isSmooth;
	bool   m_isRepeated;
};

#endif // !TEXTURE2D_HPP