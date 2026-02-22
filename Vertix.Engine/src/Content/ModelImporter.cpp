//
// Created by Natsurainko on 2026/1/25.
//

#include "Content/ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <DirectXTK12/SimpleMath.h>

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

bool Vertix::Engine::ModelImporter::TryLoadFromFile(Model &model, const std::string &filePath) {
    try {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            return false;

        ProcessNode(model, scene->mRootNode, scene);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Vertix::Engine::ModelImporter::ProcessNode(Model &model,
                                                const aiNode* node,
                                                const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.Meshes.push_back(ProcessMesh(mesh));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(model, node->mChildren[i], scene);
    }
}

Vertix::Mesh Vertix::Engine::ModelImporter::ProcessMesh(const aiMesh *aiMesh) {
    Mesh mesh;

    if (aiMesh->mName.length > 0) {
        mesh.Name = aiMesh->mName.C_Str();
    }

    for (unsigned int i = 0; i < aiMesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.Position = Vector3(
            aiMesh->mVertices[i].x,
            aiMesh->mVertices[i].y,
            aiMesh->mVertices[i].z);

        if (aiMesh->HasNormals()) {
            vertex.Normal = Vector3(
                aiMesh->mNormals[i].x,
                aiMesh->mNormals[i].y,
                aiMesh->mNormals[i].z);
        }

        if (aiMesh->HasTextureCoords(0)) {
            vertex.TexCoord = Vector2(
                aiMesh->mTextureCoords[0][i].x,
                aiMesh->mTextureCoords[0][i].y);

            vertex.Tangent = Vector3(
                aiMesh->mTangents[i].x,
                aiMesh->mTangents[i].y,
                aiMesh->mTangents[i].z);

            vertex.Bitangent = Vector3(
                aiMesh->mBitangents[i].x,
                aiMesh->mBitangents[i].y,
                aiMesh->mBitangents[i].z);
        } else
            vertex.TexCoord = Vector2(0, 0);

        mesh.Vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
        for (unsigned int j = 0; j < aiMesh->mFaces[i].mNumIndices; j++)
            mesh.Indices.push_back(aiMesh->mFaces[i].mIndices[j]);

    return mesh;
}