//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODELIMPORTER_H
#define VERTIX_MODELIMPORTER_H

#include <string>
#include <assimp/scene.h>

#include "ModelImportOptions.h"
#include "Primitive/Model.h"

namespace Vertix::Engine {
    struct ModelTextureLoadContext {
        aiTextureType Type;
        std::string FilePath;
    };

    struct ModelMaterialLoadCallbackContext {
        std::string Name;
        std::vector<ModelTextureLoadContext> Textures;
    };

    struct ModelLoadCallbackContext {
        std::unique_ptr<Model> Model = nullptr;
        std::unordered_set<unsigned int> MaterialIndices;
        std::string Name;

        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Scale;
        DirectX::SimpleMath::Quaternion Orientation;
    };

    class ModelImporter {
    public:
        [[nodiscard]]
        static bool TryLoadFromFile(Model &model,
                                    const std::string &filePath,
                                    const ModelImportOptions &options = ModelImportOptions::GetDefaultModelImportOptions());


        static bool TryLoadFromFile(const std::function<void(ModelLoadCallbackContext*)> &modelLoadCallback,
                                    const std::string &filePath,
                                    const ModelImportOptions &options = ModelImportOptions::GetDefaultModelImportOptions(),
                                    const std::function<void(ModelMaterialLoadCallbackContext*)>* modelMaterialLoadCallback = nullptr);

    private:
        static void ProcessNode(Model &model,
                                const aiNode *node,
                                const aiScene *scene,
                                const ModelImportOptions &options,
                                const aiMatrix4x4t<float> &parentTransformation);

        static void ProcessNode(const std::function<void(ModelLoadCallbackContext *)> &modelLoadCallback,
                                const aiNode *node,
                                const aiScene *scene,
                                const ModelImportOptions &options,
                                const aiMatrix4x4t<float> &parentTransformation);

        static void ProcessMaterial(const aiScene *scene, const std::function<void(ModelMaterialLoadCallbackContext*)> &modelMaterialLoadCallback);

        static void ProcessMesh(const aiMesh *aiMesh,
                                Mesh &mesh,
                                const aiMatrix4x4t<float> &transformation);
    };
}

#endif //VERTIX_MODELIMPORTER_H