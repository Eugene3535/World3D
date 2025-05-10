#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>
#include <span>


struct Mesh
{
    uint32_t vbo = 0;
    uint32_t ebo = 0;
    uint32_t vao = 0;
    uint32_t indexCount = 0;

    std::span<const class Texture> textures;
};

#endif // !MESH_HPP