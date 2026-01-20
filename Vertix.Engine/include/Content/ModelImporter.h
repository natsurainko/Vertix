//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODELIMPORTER_H
#define VERTIX_MODELIMPORTER_H

#include <string>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Primitive/Model.h"

namespace Vertix::Engine {
    class ModelImporter {
    public:
        static bool TryLoadFromFile(Model &model, const std::string &filePath) {
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

    private:
        static void ProcessNode(Model &model, const aiNode* node, const aiScene* scene) {
            for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
                const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                model.Meshes.push_back(ProcessMesh(mesh));
            }

            for (unsigned int i = 0; i < node->mNumChildren; ++i) {
                ProcessNode(model, node->mChildren[i], scene);
            }
        }

        static Mesh ProcessMesh(const aiMesh *aiMesh) {
            Mesh mesh;

            if (aiMesh->mName.length > 0) {
                mesh.Name = aiMesh->mName.C_Str();
            }

            for (unsigned int i = 0; i < aiMesh->mNumVertices; ++i) {
                Vertex vertex;
                vertex.Position = DirectX::SimpleMath::Vector3(
                    aiMesh->mVertices[i].x,
                    aiMesh->mVertices[i].y,
                    aiMesh->mVertices[i].z);

                if (aiMesh->HasNormals()) {
                    vertex.Normal = DirectX::SimpleMath::Vector3(
                        aiMesh->mNormals[i].x,
                        aiMesh->mNormals[i].y,
                        aiMesh->mNormals[i].z);
                }

                if (aiMesh->HasTextureCoords(0)) {
                    vertex.TexCoord = DirectX::SimpleMath::Vector2(
                        aiMesh->mTextureCoords[0][i].x,
                        aiMesh->mTextureCoords[0][i].y);

                    vertex.Tangent = DirectX::SimpleMath::Vector3(
                        aiMesh->mTangents[i].x,
                        aiMesh->mTangents[i].y,
                        aiMesh->mTangents[i].z);

                    vertex.Bitangent = DirectX::SimpleMath::Vector3(
                        aiMesh->mBitangents[i].x,
                        aiMesh->mBitangents[i].y,
                        aiMesh->mBitangents[i].z);
                } else
                    vertex.TexCoord = DirectX::SimpleMath::Vector2(0, 0);

                mesh.Vertices.push_back(vertex);
            }

            for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
                for (unsigned int j = 0; j < aiMesh->mFaces[i].mNumIndices; j++)
                    mesh.Indices.push_back(aiMesh->mFaces[i].mIndices[j]);

            return mesh;
        }
    };
}

#endif //VERTIX_MODELIMPORTER_H