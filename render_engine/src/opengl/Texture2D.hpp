#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <filesystem>

#include "opengl/GlResource.hpp"

class RE_API Texture2D final:
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

	Texture2D(const std::filesystem::path& filePath, WrapMode wrap, FilterMode filter) noexcept;
	Texture2D(const class Image& image, WrapMode wrap, FilterMode filter) noexcept;
	Texture2D(const Texture2D&) noexcept = delete;
	Texture2D(Texture2D&&) noexcept;
	Texture2D& operator = (const Texture2D&) noexcept = delete;
	Texture2D& operator = (Texture2D&&) noexcept;
	~Texture2D() noexcept;

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