#include <ft2build.h>
#include FT_FREETYPE_H

#include <stb_image_write.h>

#include "files/FileProvider.hpp"
#include "files/Font.hpp"


static constexpr char utf8[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя0123456789!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~ ";
static std::wstring utf16;


static wchar_t* utf8_to_wchar(const char* utf8_str) noexcept
{
    size_t len = strlen(utf8_str);
    wchar_t* wstr = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));

    if (!wstr) 
        return NULL;
    
    size_t i = 0, wpos = 0;
    
    while (i < len) 
    {
        uint32_t codepoint;
        uint8_t byte = (uint8_t)utf8_str[i++];
        
        if ((byte & 0x80) == 0) // 1-byte sequence (ASCII)
        {
            codepoint = byte;
        } 
        else if ((byte & 0xE0) == 0xC0 && i < len) // 2-byte sequence
        {
            codepoint = ((byte & 0x1F) << 6) | ((uint8_t)utf8_str[i++] & 0x3F);
        } 
        else if ((byte & 0xF0) == 0xE0 && i + 1 < len) // 3-byte sequence
        {
            codepoint = ((byte & 0x0F) << 12) | 
                      (((uint8_t)utf8_str[i++] & 0x3F) << 6) | 
                      ((uint8_t)utf8_str[i++] & 0x3F);
        } 
        else if ((byte & 0xF8) == 0xF0 && i + 2 < len) // 4-byte sequence
        {
            codepoint = ((byte & 0x07) << 18) | 
                      (((uint8_t)utf8_str[i++] & 0x3F) << 12) |
                      (((uint8_t)utf8_str[i++] & 0x3F) << 6) |
                      ((uint8_t)utf8_str[i++] & 0x3F);
                      
            // If wchar_t does not support the full Unicode domain, it is necessary to encode surrogate pairs of
            if (sizeof(wchar_t) < 4 && codepoint > 0xFFFF) 
            {
                codepoint -= 0x10000;
                wstr[wpos++] = (wchar_t)(0xD800 | ((codepoint >> 10) & 0x3FF));
                wstr[wpos++] = (wchar_t)(0xDC00 | (codepoint & 0x3FF));
                continue;
            }
        } 
        else // Incorrect UTF-8 sequence, replace with a question mark
        {
            wstr[wpos++] = L'?';
            continue;
        }
        
        wstr[wpos++] = (wchar_t)codepoint;
    }
    
    wstr[wpos] = L'\0';
    return wstr;
}


Font::Font() noexcept:
    m_library(nullptr),
    m_face(nullptr)
{
    if(utf16.empty())
    {
        wchar_t* converted = utf8_to_wchar(utf8);
        utf16 = std::wstring(converted);
        free(converted);
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

    if(auto image = getImage(characterSize); image.first)
        result = stbi_write_png(m_info.family.c_str(), image.second.x, image.second.y, 1, image.first, 0);

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
        page.size = { 128, 128 };

        int amount = 0;
    
        for(const auto wc : utf16)
            if(FT_Load_Char(face, wc, FT_LOAD_RENDER) == 0)
            {
                writeGlyphToPage(wc, page);
                ++amount;
            }

        const glm::vec2 size = page.size;

        for(auto& [wc, glyph] : page.glyphs)
        {
            glm::vec4 textureRect = glyph.imageRect;

            // padding
            textureRect.x -= 2.f;
            textureRect.y -= 2.f;

            float left   = textureRect.x / size.x;
            float top    = textureRect.y / size.y;
            float right  = (textureRect.x + textureRect.z) / size.x;
            float bottom = (textureRect.y + textureRect.w) / size.y;

            glyph.textureRect = { left, top, right, bottom };
        }
 
        return amount > 0;
    }

    return false;
}


bool Font::isLoaded() const noexcept
{
    return (m_library != nullptr && m_face != nullptr);
}


std::pair<const uint8_t*, glm::ivec2> Font::getImage(uint32_t characterSize) const noexcept
{
    std::pair<const uint8_t*, glm::ivec2> image = { nullptr, glm::ivec2(0, 0) };

    if(auto it = m_pages.find(characterSize); it != m_pages.end())
    {
        image.first = it->second.image.data();
        image.second = it->second.size;
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
            page.size = { newImageWidth, newImageHeight };
        }

        page.rows.emplace_back(page.nextRow, rowHeight);
        page.nextRow += rowHeight;
        row = &page.rows.back();
    }

    glm::ivec4 rect(row->width, row->top, width, height);
    row->width += width;

    return rect;
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
    
        auto imageRect = findGlyphRect(page, width, height);
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

        glyph.bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top };
        glyph.size    = { bitmap.width, bitmap.rows };
        glyph.advance = face->glyph->advance.x;
    }
}