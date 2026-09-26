#pragma once

#include <vector>

#include <cglm/struct/ivec2.h>

#include "GfxApiExport.hpp"



class GFX_API Grid3D
{
public:
    bool create() noexcept;

    ivec2s cellCount;
    bool isTiled;

    struct
    {
        ivec2s size;
        bool isEnabled;
        bool isRepeated;
    } texture;

    std::vector<float>    vertices;
    std::vector<uint32_t> indices;
};
