//
// Created by Natsurainko on 2026/1/25.
//

#include "Content/ModelLoader.h"

#include <assimp/Importer.hpp>
#include <DirectXTK12/SimpleMath.h>

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

const aiMatrix4x4t aiMatrix_Identity = {
    1.0f,0.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,0.0f,
    0.0f,0.0f,1.0f,0.0f,
    0.0f,0.0f,0.0f,1.0f
};

bool Vertix::Engine::ModelLoader::TryLoadFromFile(
    const std::function<void(ModelLoadCallbackContext*)>& modelLoadCallback,
    const std::string &filePath,
    const ModelLoadOptions &options,
    const std::function<void(ModelMaterialLoadCallbackContext*)>* modelMaterialLoadCallback)
{
    ModelLoadingContext loadingContext {
        .importOptions = options,
        .modelLoadCallback = &modelLoadCallback,
        .materialLoadCallback = modelMaterialLoadCallback
    };

    try {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath, options.AssimpPostProcessSteps);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            return false;

        if (modelMaterialLoadCallback) {
            ProcessMaterial(scene, &loadingContext);
        }

        ProcessNode(&loadingContext, scene->mRootNode, scene, aiMatrix_Identity);
    } catch (const std::exception& e) {
        return false;
    }

    return true;
}

void Vertix::Engine::ModelLoader::ProcessNode(
    const ModelLoadingContext* loadingContext,
    const aiNode* node,
    const aiScene* scene,
    const aiMatrix4x4t<float> &parentTransformation)
{
    if (node->mNumMeshes > 0) {
        ModelLoadCallbackContext callbackContext = {
            .Model = std::make_unique<Model>(),
            .Name = node->mName.length > 0 ? node->mName.C_Str() : "UnnamedModel",
            .Position = Vector3::Zero,
            .Scale = Vector3::One,
            .Orientation = DirectX::SimpleMath::Quaternion::Identity,
        };

        const auto transformation = loadingContext->importOptions.ApplyTransformationToModel
            ? parentTransformation * node->mTransformation
            : node->mTransformation;

        aiQuaternion aiRotation;
        transformation.Decompose(
            *reinterpret_cast<aiVector3D*>(&callbackContext.Scale),
            aiRotation,
            *reinterpret_cast<aiVector3D*>(&callbackContext.Position)
        );
        callbackContext.Orientation = DirectX::SimpleMath::Quaternion(
            aiRotation.x,
            aiRotation.y,
            aiRotation.z,
            aiRotation.w
        );

        auto* model = callbackContext.Model.get();
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            ProcessMesh(
                scene->mMeshes[node->mMeshes[i]],
                model->Meshes.emplace_back(),
                aiMatrix_Identity,
                loadingContext
            );
        }

        (*loadingContext->modelLoadCallback)(&callbackContext);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(loadingContext, node->mChildren[i], scene, parentTransformation * node->mTransformation);
    }
}

void Vertix::Engine::ModelLoader::ProcessMaterial(
    const aiScene *scene,
    ModelLoadingContext* loadingContext)
{
    for (unsigned int mi = 0; mi  < scene->mNumMaterials; mi ++) {
        const aiMaterial* mat = scene->mMaterials[mi];

        aiString matName;
        mat->Get(AI_MATKEY_NAME, matName);

        ModelMaterialLoadCallbackContext callbackContext {
            .Name = matName.length > 0 ? matName.C_Str() : "UnnamedMaterial",
        };

        for (int t = 1; t < aiTextureType_GLTF_METALLIC_ROUGHNESS; ++t) {
            const auto type = static_cast<aiTextureType>(t);

            for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
                aiString path;
                mat->GetTexture(type, i, &path);

                if (const bool embedded = path.length > 0 && path.C_Str()[0] == '*'; !embedded) {
                    callbackContext.Textures.emplace_back(ModelTextureLoadContext {
                        .Type = type,
                        .FilePath = path.C_Str(),
                    });
                }
            }
        }

        (*loadingContext->materialLoadCallback)(&callbackContext);
        loadingContext->materialHandles.emplace_back(callbackContext.MaterialHandle);
    }
}

void Vertix::Engine::ModelLoader::ProcessMesh(
    const aiMesh *aiMesh,
    Mesh &mesh,
    const aiMatrix4x4t<float> &transformation,
    const ModelLoadingContext* loadingContext)
{
    if (aiMesh->mName.length > 0) {
        mesh.Name = aiMesh->mName.C_Str();
    }

    if (aiMesh->mMaterialIndex < loadingContext->materialHandles.size()) {
        mesh.Material = loadingContext->materialHandles[aiMesh->mMaterialIndex];
    }

    const size_t baseIndexCount = mesh.Indices.size();
    const size_t baseVertexCount = mesh.Vertices.size();

    mesh.Indices.reserve(baseIndexCount + aiMesh->mNumFaces * 3);
    mesh.Vertices.reserve(baseVertexCount + aiMesh->mNumVertices);

    for (unsigned int i = 0; i < aiMesh->mNumFaces; ++i)
        for (unsigned int j = 0; j < aiMesh->mFaces[i].mNumIndices; ++j)
            mesh.Indices.push_back(aiMesh->mFaces[i].mIndices[j] + baseVertexCount);

    for (unsigned int i = 0; i < aiMesh->mNumVertices; ++i) {
        Vertex &vertex = mesh.Vertices.emplace_back();

        aiMesh->mVertices[i] = transformation * aiMesh->mVertices[i];
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

        if (aiMesh->HasTangentsAndBitangents()) {
            vertex.Tangent = Vector3(
                aiMesh->mTangents[i].x,
                aiMesh->mTangents[i].y,
                aiMesh->mTangents[i].z);

            vertex.Bitangent = Vector3(
                aiMesh->mBitangents[i].x,
                aiMesh->mBitangents[i].y,
                aiMesh->mBitangents[i].z);
        }

        if (aiMesh->HasTextureCoords(0)) {
            vertex.TexCoord = Vector2(
                aiMesh->mTextureCoords[0][i].x,
                aiMesh->mTextureCoords[0][i].y);
        }
    }
}
