#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <filesystem>

#include "opengl/resources/GlResource.hpp"

class OGL_API Texture2D final:
	public GlResource
{
public:
	enum class WrapMode
	{
		Repeat,
		ClampToBorder
	};

	enum class FilterMode
	{
		Nearest,
		Linear
	};

	Texture2D(uint32_t handle) noexcept;
	~Texture2D() noexcept;

	bool loadFromImage(const class Image& image, WrapMode wrap, FilterMode filter) noexcept;
	bool loadFromFile(const std::filesystem::path& filePath, WrapMode wrap, FilterMode filter) noexcept;

	void setSmooth(bool smooth)    noexcept;
	void setRepeated(bool repeate) noexcept;

	bool isSmooth()   const noexcept;
	bool isRepeated() const noexcept;

	int32_t getWidth()  const noexcept;
	int32_t getHeight() const noexcept;

	static void bind(const Texture2D* texture) noexcept;
	static void enable(uint32_t unit) noexcept;

private:
	int32_t m_width;
    int32_t m_height;

	bool m_isSmooth;
	bool m_isRepeated;
};

#endif // !TEXTURE2D_HPP