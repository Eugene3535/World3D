#include <cstring>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include "loaders/MeshLoader.hpp"

bool MeshLoader::load_meshes_from_file(const std::filesystem::path& fpath, std::vector<VertexBuffer>& meshes) noexcept
{
    meshes.clear();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fpath.generic_string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
#ifdef DEBUG
        auto err = importer.GetErrorString();
#endif
        return false;
    } 
        
    process_node(scene->mRootNode, scene, meshes);
    
    return true;
}

void MeshLoader::process_node(aiNode* node, const aiScene* scene, std::vector<VertexBuffer>& meshes) noexcept
{
    for(glm::u32 i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    for(glm::u32 i = 0; i < node->mNumChildren; ++i)
        process_node(node->mChildren[i], scene, meshes); 
}

VertexBuffer MeshLoader::process_mesh(const aiMesh* mesh, const aiScene* scene) noexcept
{
    std::vector<Vertex> vertices;
    std::vector<glm::u32> indices;
    glm::u32 vertex_count = mesh->mNumVertices;
    vertices.resize(vertex_count);

    if(mesh->HasPositions())
    {
        for(glm::u32 i = 0; i < vertex_count; ++i)
        {
            Vertex& vertex = vertices[i];

            vertex.position.x = static_cast<float>(mesh->mVertices[i].x);
            vertex.position.y = static_cast<float>(mesh->mVertices[i].y);
            vertex.position.z = static_cast<float>(mesh->mVertices[i].z);
        }
    }

    if(mesh->HasTextureCoords(0)) 
    {
        for(glm::u32 i = 0; i < vertex_count; ++i)
        {
            Vertex& vertex = vertices[i];

            vertex.uv.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
            vertex.uv.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
        }
    }

    if (mesh->HasNormals())
    {
        for(glm::u32 i = 0; i < vertex_count; ++i)
        {
            Vertex& vertex = vertices[i];

            vertex.normal.x = static_cast<float>(mesh->mNormals[i].x);
            vertex.normal.y = static_cast<float>(mesh->mNormals[i].y);
            vertex.normal.z = static_cast<float>(mesh->mNormals[i].z);
        }
    }

    const aiFace* faces = mesh->mFaces;
    const glm::u32 face_count = mesh->mNumFaces;
    glm::u32 index_count = 0;

    for(glm::u32 i = 0; i < face_count; ++i)  
        index_count += faces[i].mNumIndices;

    indices.resize(index_count, 0);
    index_count = 0;

    for(glm::u32 i = 0; i < face_count; ++i)
    {
        glm::u32* dst = indices.data() + index_count;
        const glm::u32* src = faces[i].mIndices;
        const std::size_t byte_count = sizeof(unsigned) * faces[i].mNumIndices;

        std::memcpy(dst, src, byte_count);

        index_count += faces[i].mNumIndices;
    }

    VertexBuffer vb;
    vb.create(vertices, indices);

    return vb;
}