#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <filesystem>

class Texture2D
{
public:
	Texture2D(const std::filesystem::path& filePath, int32_t wrap, int32_t filter) noexcept;
	Texture2D(const class Image& image, int32_t wrap, int32_t filter) noexcept;
	Texture2D(const Texture2D&) noexcept = delete;
	Texture2D(Texture2D&&) noexcept;
	Texture2D& operator = (const Texture2D&) noexcept = delete;
	Texture2D& operator = (Texture2D&&) noexcept;
	~Texture2D() noexcept;

	void setSmooth(bool smooth)    noexcept;
	void setRepeated(bool repeate) noexcept;

	bool isSmooth()   const noexcept;
	bool isRepeated() const noexcept;
	bool isValid()    const noexcept;

	uint32_t getHandle() const noexcept;
	int32_t  width()     const noexcept;
	int32_t  height()    const noexcept;

	static void bind(const Texture2D* texture) noexcept;

private:
	uint32_t m_handle;
	int32_t  m_width;
    int32_t  m_height;

	bool m_isSmooth;
	bool m_isRepeated;
};

#endif // !TEXTURE2D_HPP