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

	GLuint getHandle() const noexcept;
	GLint  getWidth()  const noexcept;
	GLint  getHeight() const noexcept;

	void setSmooth(bool smooth)    noexcept;
	void setRepeated(bool repeate) noexcept;

	bool isSmooth()   const noexcept;
	bool isRepeated() const noexcept;

private:
	const GLuint m_handle;
	GLint  m_width;
    GLint  m_height;
	bool   m_isSmooth;
	bool   m_isRepeated;
};

#endif // !TEXTURE2D_HPP