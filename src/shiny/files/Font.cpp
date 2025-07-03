#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <utf8.h>

#include "files/FileProvider.hpp"
#include "files/Font.hpp"


static constexpr char utf8_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя0123456789!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~ ";
std::wstring Font::wide_chars;


static std::wstring utf8_to_wstring(const std::string& utf8_str) 
{
    std::wstring wstr;

    if constexpr (sizeof(wchar_t) == 2) 
    {// UTF-16 (Windows)
        std::u16string utf16;
        utf8::utf8to16(utf8_str.begin(), utf8_str.end(), std::back_inserter(utf16));
        wstr.assign(utf16.begin(), utf16.end());
    } 
    else 
    {// UTF-32 (Linux/macOS)
        std::u32string utf32;
        utf8::utf8to32(utf8_str.begin(), utf8_str.end(), std::back_inserter(utf32));
        wstr.assign(utf32.begin(), utf32.end());
    }

    return wstr;
}


Font::Font() noexcept:
    m_library(nullptr),
    m_face(nullptr)
{
    if(Font::wide_chars.empty())
    {
        Font::wide_chars = utf8_to_wstring(utf8_chars);
    }
}


Font::~Font() noexcept
{
    cleanup();
}


bool Font::loadFromFile(const std::string& font_name) noexcept
{
    cleanup();

    auto file_path = FileProvider::findPathToFile(font_name);

    if(FT_Library library; FT_Init_FreeType(&library) == 0)
    {
        if(FT_Face face; FT_New_Face(library, file_path.generic_string().c_str(), 0, &face) == 0)
        {
            m_library = library;
            m_face = face;

            if(face->family_name)
                m_info.family = face->family_name;
            else
                m_info.family = font_name;

            return true;
        }
        
        FT_Done_FreeType(library);
    }

    return false;
}


bool Font::writeToFile(uint32_t characterSize) const noexcept
{
    bool result = false;

    if(auto image = getImage(characterSize); image.pixels != nullptr)
        result = stbi_write_png(m_info.family.c_str(), image.width, image.height, 1, image.pixels, 0);

    return result;
}


bool Font::loadPage(uint32_t characterSize) noexcept
{
    if(auto page = m_pages.find(characterSize); page != m_pages.end())
        return true;

    if(characterSize == 0)
        return false;

    if(isLoaded())
    {
        FT_Face face = static_cast<FT_Face>(m_face);
        FT_Set_Pixel_Sizes(face, 0, characterSize);

        auto& page = m_pages[characterSize]; 
        page.image.resize(128 * 128);
        page.size = {128, 128 };

        int amount = 0;
    
        for(const auto wc : wide_chars)
            if(FT_Load_Char(face, wc, FT_LOAD_RENDER) == 0)
            {
                writeGlyphToPage(wc, page);
                ++amount;
            }

        glm::vec2 size(page.size);

        for(auto& [wc, glyph] : page.glyphs)
        {
            glm::vec4 textureRect(glyph.imageRect); 

            // padding
            textureRect.x -= 2.f;
            textureRect.y -= 2.f;

            float left   = textureRect.x / size.x;
            float top    = textureRect.y / size.y;
            float right  = (textureRect.x + textureRect.z) / size.x;
            float bottom = (textureRect.y + textureRect.w) / size.y;

            glyph.textureRect = { left, top, right, bottom };
        }
 
        return (amount > 0);
    }

    return false;
}


bool Font::isLoaded() const noexcept
{
    return (m_library != nullptr && m_face != nullptr);
}


Font::Image Font::getImage(uint32_t characterSize) const noexcept
{
    Image image = {};

    if(auto it = m_pages.find(characterSize); it != m_pages.end())
    {
        image.pixels        = it->second.image.data();
        image.width         = it->second.size.x;
        image.height        = it->second.size.y;
        image.characterSize = it->first;
    }
        
    return image;
}


const Font::Info& Font::getInfo() const noexcept
{
    return m_info;
}


bool Font::hasGlyph(wchar_t codePoint) const noexcept
{
    if(m_face)
        return FT_Get_Char_Index(static_cast<FT_Face>(m_face), codePoint) != 0;

    return false;
}


void Font::getGlyph(wchar_t codePoint, uint32_t characterSize, Glyph* glyph) const noexcept
{
    if(auto it = m_pages.find(characterSize); it != m_pages.end())
    {
        if(auto found = it->second.glyphs.find(codePoint); found != it->second.glyphs.end())
        {
            memcpy(glyph, &found->second, sizeof(Glyph));
        }
    }
}


GlyphTable Font::getGlyphs(uint32_t characterSize) const noexcept
{
    if(auto it = m_pages.find(characterSize); it != m_pages.end())
        return it->second.glyphs;

    return {};
}


void Font::cleanup() noexcept
{
    if(m_face)
    {
        FT_Done_Face(static_cast<FT_Face>(m_face));
        m_face = nullptr;
    }

    if(m_library)
    {
        FT_Done_FreeType(static_cast<FT_Library>(m_library));
        m_library = nullptr;
    }

    m_info.family.clear();
}


glm::ivec4 Font::findGlyphRect(Font::Page& page, uint32_t width, uint32_t height) noexcept
{
    Font::Row* row = nullptr;
    float bestRatio = 0;
    glm::ivec4 result(0);

    for (auto it = page.rows.begin(); it != page.rows.end() && !row; ++it)
    {
        float ratio = static_cast<float>(height) / static_cast<float>(it->height);

        if ((ratio < 0.7f) || (ratio > 1.f))
            continue;

        if (width > page.size.x - it->width)
            continue;

        if (ratio < bestRatio)
            continue;

        row = &*it;
        bestRatio = ratio;
    }

    if (!row) 
    {
        uint32_t rowHeight = height + height / 10;

        if((page.nextRow + rowHeight >= page.size.y) || (width >= page.size.x)) // Make the image 2 times bigger
        {
            uint32_t imageWidth     = page.size.x;
            uint32_t imageHeight    = page.size.y;
            uint32_t newImageWidth  = imageWidth << 1;
            uint32_t newImageHeight = imageHeight << 1;

            std::vector<uint8_t> newImage(newImageWidth * newImageHeight);

            const uint8_t* oldPixels = page.image.data();
            uint8_t* newPixels = newImage.data();

            for (uint32_t i = 0; i < imageHeight; ++i)
            {
                memcpy(newPixels, oldPixels, imageWidth);
                oldPixels += imageWidth;
                newPixels += newImageWidth;
            }

            page.image.swap(newImage);
            page.size.x = newImageWidth;
            page.size.y = newImageHeight;
        }

        page.rows.emplace_back(page.nextRow, rowHeight);
        page.nextRow += rowHeight;
        row = &page.rows.back();
    }

    result = { row->width, row->top, width, height };
    row->width += width;

    return result;
}


void Font::writeGlyphToPage(wchar_t wc, Font::Page& page) noexcept
{
    FT_Face face = static_cast<FT_Face>(m_face);
    const auto& bitmap = face->glyph->bitmap;

    if(auto it = page.glyphs.find(wc); it == page.glyphs.end())
    {
        Glyph& glyph = page.glyphs[wc];

        uint32_t width  = bitmap.width;
        uint32_t height = bitmap.rows;
    
        const uint32_t rows = bitmap.rows;
        const uint32_t columns = bitmap.width;
        const uint32_t padding = 2;
    
        width  += (padding << 1);
        height += (padding << 1);
    
        glm::ivec4 imageRect = findGlyphRect(page, width, height);
        glyph.imageRect = imageRect;

        glyph.imageRect.x += static_cast<int>(padding);
        glyph.imageRect.y += static_cast<int>(padding);
        glyph.imageRect.z -= static_cast<int>(padding << 1);
        glyph.imageRect.w -= static_cast<int>(padding << 1);
    
        uint32_t stride = page.size.x;
        const uint8_t* srcPixels = bitmap.buffer;
        uint8_t*       dstPixels = page.image.data() + (imageRect.y * stride + imageRect.x);
    
        for (uint32_t i = 0; i < rows; ++i)
        {
            memcpy(dstPixels, srcPixels, columns);
            srcPixels += columns;
            dstPixels += stride;
        }

        glyph.bearing.x = face->glyph->bitmap_left; 
        glyph.bearing.y = face->glyph->bitmap_top;
        glyph.size.x    = bitmap.width;
        glyph.size.y    = bitmap.rows;
        glyph.advance   = face->glyph->advance.x;
    }
}