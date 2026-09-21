#include "geometry/GeometryGenerator3D.hpp"


bool GeometryGenerator3D::createGrid(Grid3D& grid) noexcept
{
    if (!(grid.cellCount.x && grid.cellCount.y))
        return false;

    size_t vertexCount = 0;

    if (grid.isTiled)
    {
        vertexCount = grid.cellCount.x * grid.cellCount.y * 4;
    }
    else
    {
        vertexCount = (grid.cellCount.x + 1) * (grid.cellCount.y + 1);
    }

    const size_t vertexComponentCount = grid.texture.isEnabled ? 5 : 3;
    grid.vertices.resize(vertexCount * vertexComponentCount);
    grid.indices.resize(grid.cellCount.x * grid.cellCount.y * 6);

    auto fillTexturedTiledGrid = [&grid, vertexComponentCount]() -> void
    {
        const uint32_t width = grid.cellCount.x;
        const uint32_t height = grid.cellCount.y;

        float* vertices = grid.vertices.data();
        uint32_t* indices = grid.indices.data();
        uint32_t index = 0;

        for (uint32_t i = 0; i < height; ++i)
        {
            for (uint32_t j = 0; j < width; ++j)
            {
                vertices[0] = static_cast<float>(i);
                vertices[2] = static_cast<float>(j);
                vertices[3] = 0.f;
                vertices[4] = 0.f;
                vertices += vertexComponentCount;

                vertices[0] = static_cast<float>(i);
                vertices[2] = static_cast<float>(j + 1);
                vertices[3] = 1.f;
                vertices[4] = 0.f;
                vertices += vertexComponentCount;

                vertices[0] = static_cast<float>(i + 1);
                vertices[2] = static_cast<float>(j + 1);
                vertices[3] = 1.f;
                vertices[4] = 1.f;
                vertices += vertexComponentCount;

                vertices[0] = static_cast<float>(i + 1);
                vertices[2] = static_cast<float>(j);
                vertices[3] = 0.f;
                vertices[4] = 1.f;
                vertices += vertexComponentCount;

                indices[0] = index;
                indices[1] = index + 1;
                indices[2] = index + 2;
                indices[3] = index;
                indices[4] = index + 2;
                indices[5] = index + 3;

                index += 6; // shift to next cell
            }
        }
    };

    if (grid.isTiled && grid.texture.isEnabled && !grid.texture.isRepeated)
        fillTexturedTiledGrid();
    
    return true;
}

