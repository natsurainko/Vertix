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
    struct ModelLoadOptions {
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

        MaterialHandle MaterialHandle;
    };

    struct ModelLoadCallbackContext {
        std::unique_ptr<Model> Model = nullptr;
        std::string Name;

        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Scale;
        DirectX::SimpleMath::Quaternion Orientation;
    };

    class ModelLoader {
    public:
        struct ModelLoadingContext {
            ModelLoadOptions importOptions;
            const std::function<void(ModelLoadCallbackContext*)>* modelLoadCallback;
            const std::function<void(ModelMaterialLoadCallbackContext*)>* materialLoadCallback;
            std::vector<MaterialHandle> materialHandles;
        };

        static VERTIX_ENGINE_API bool TryLoadFromFile(
            const std::function<void(ModelLoadCallbackContext*)> &modelLoadCallback,
            const std::string &filePath,
            const ModelLoadOptions &options = ModelLoadOptions{},
            const std::function<void(ModelMaterialLoadCallbackContext*)>* modelMaterialLoadCallback = nullptr);

    private:
        static void ProcessNode(
            const ModelLoadingContext *loadingContext,
            const aiNode *node,
            const aiScene *scene, const aiMatrix4x4t<float> &parentTransformation);

        static void ProcessMaterial(
            const aiScene *scene, ModelLoadingContext *loadingContext);

        static void ProcessMesh(
            const ::aiMesh *aiMesh,
            Mesh &mesh,
            const aiMatrix4x4t<float> &transformation, const ModelLoadingContext *loadingContext);
    };
}

#endif //VERTIX_MODELIMPORTER_H