#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <filesystem>
#include <vector>

#include "graphics/VertexBuffer.hpp"

class MeshLoader
{
public:
	bool load_meshes_from_file(const std::filesystem::path& fpath, std::vector<VertexBuffer>& meshes) noexcept;

private:
    void process_node(struct aiNode* node, const struct aiScene* scene, std::vector<VertexBuffer>& meshes) noexcept;
    VertexBuffer process_mesh(const struct aiMesh* mesh, const struct aiScene* scene) noexcept;
};

#endif // !MESH_LOADER_HPP