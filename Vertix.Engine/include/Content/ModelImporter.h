//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODELIMPORTER_H
#define VERTIX_MODELIMPORTER_H

#include <string>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "VERTIX_ENGINE_EXPORT.h"
#include "Primitive/Model.h"

namespace Vertix::Engine {
    struct ModelImportOptions {
        bool ApplyTransformationToModel = true;
        unsigned int AssimpPostProcessSteps =
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_CalcTangentSpace |
                aiProcess_MakeLeftHanded |
                aiProcess_OptimizeMeshes;
    };

    struct ModelTextureLoadContext {
        aiTextureType Type;
        std::string FilePath;
    };

    struct ModelMaterialLoadCallbackContext {
        std::string Name;
        std::vector<ModelTextureLoadContext> Textures;
    };

    struct ModelMeshProcessCallbackContext {
        Mesh* Mesh;
        unsigned int MaterialIndex;
    };

    struct ModelLoadCallbackContext {
        std::unique_ptr<Model> Model = nullptr;
        std::string Name;

        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Scale;
        DirectX::SimpleMath::Quaternion Orientation;
    };

    class ModelImporter {
    public:
        static VERTIX_ENGINE_API bool TryLoadFromFile(
            const std::function<void(ModelLoadCallbackContext*)> &modelLoadCallback,
            const std::string &filePath,
            const ModelImportOptions &options = ModelImportOptions{},
            const std::function<void(ModelMaterialLoadCallbackContext*)>* modelMaterialLoadCallback = nullptr,
            const std::function<void(ModelMeshProcessCallbackContext*)>* modelMeshProcessCallback = nullptr);

        /*[[nodiscard]]
        static bool TryLoadFromFile(Model &model,
                                    const std::string &filePath,
                                    const ModelImportOptions &options = ModelImportOptions::GetDefaultModelImportOptions());*/
    private:
        static void ProcessNode(
            const std::function<void(ModelLoadCallbackContext *)> &modelLoadCallback,
            const aiNode *node,
            const aiScene *scene,
            const ModelImportOptions &options,
            const aiMatrix4x4t<float> &parentTransformation,
            const std::function<void(ModelMeshProcessCallbackContext*)>* modelMeshProcessCallback = nullptr);

        static void ProcessMaterial(
            const aiScene *scene,
            const std::function<void(ModelMaterialLoadCallbackContext*)> &modelMaterialLoadCallback);

        static void ProcessMesh(
            const aiMesh *aiMesh,
            Mesh &mesh,
            const aiMatrix4x4t<float> &transformation,
            const std::function<void(ModelMeshProcessCallbackContext*)>* modelMeshProcessCallback = nullptr);

        /*static void ProcessNode(Model &model,
                                const aiNode *node,
                                const aiScene *scene,
                                const ModelImportOptions &options,
                                const aiMatrix4x4t<float> &parentTransformation);*/
    };
}

#endif //VERTIX_MODELIMPORTER_H