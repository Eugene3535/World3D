#include "geometry/Geometry3D.hpp"


bool Grid3D::create() noexcept
{
    if (!(cellCount.x && cellCount.y))
        return false;

    size_t vertexCount = 0;

    if (isTiled)
    {
        vertexCount = cellCount.x * cellCount.y * 4;
    }
    else
    {
        vertexCount = (cellCount.x + 1) * (cellCount.y + 1);
    }

    const size_t vertexComponentCount = texture.isEnabled ? 5 : 3;
    vertices.resize(vertexCount * vertexComponentCount);
    indices.resize(cellCount.x * cellCount.y * 6);

    auto fillTexturedTiledGrid = [this, vertexComponentCount]() -> void
    {
        const uint32_t width = cellCount.x;
        const uint32_t height = cellCount.y;

        float* vertexData = vertices.data();
        uint32_t index = 0;
        uint32_t cell = 0;

        for (uint32_t i = 0; i < height; ++i)
        {
            for (uint32_t j = 0; j < width; ++j)
            {
                vertexData[0] = static_cast<float>(i);
                vertexData[2] = static_cast<float>(j);
                vertexData[3] = 0.f;
                vertexData[4] = 0.f;
                vertexData += vertexComponentCount;

                vertexData[0] = static_cast<float>(i);
                vertexData[2] = static_cast<float>(j + 1);
                vertexData[3] = 1.f;
                vertexData[4] = 0.f;
                vertexData += vertexComponentCount;

                vertexData[0] = static_cast<float>(i + 1);
                vertexData[2] = static_cast<float>(j + 1);
                vertexData[3] = 1.f;
                vertexData[4] = 1.f;
                vertexData += vertexComponentCount;

                vertexData[0] = static_cast<float>(i + 1);
                vertexData[2] = static_cast<float>(j);
                vertexData[3] = 0.f;
                vertexData[4] = 1.f;
                vertexData += vertexComponentCount;

                indices[index + 0] = cell;
                indices[index + 1] = cell + 1;
                indices[index + 2] = cell + 2;
                indices[index + 3] = cell;
                indices[index + 4] = cell + 2;
                indices[index + 5] = cell + 3;

                index += 6; // shift to next cell
                cell += 4;
            }
        }
    };

    if (isTiled && texture.isEnabled && !texture.isRepeated)
        fillTexturedTiledGrid();
    
    return true;
}

