#pragma once

#include <vector>

#include <cglm/struct/ivec2.h>


struct RoundedRect2D
{
    float x; 
    float y; 
    float width;
    float height;
    float radius;
};


struct Grid3D
{
    ivec2s cellCount;
    bool isTiled;

    struct
    {
        ivec2s size;
        bool isEnabled;
        bool isRepeated;
    } texture;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};