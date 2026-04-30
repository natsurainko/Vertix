//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODELIMPORTER_H
#define VERTIX_MODELIMPORTER_H

#include <functional>
#include <string>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Vertix/Dispatching/DispatcherQueue.hpp>
#include <Vertix/Graphics/GraphicsDevice.h>
#include <Vertix/Pool/ModelPool.hpp>
#include <Vertix/Primitive/Material.h>
#include <Vertix/Primitive/Model.h>

#include "Vertix.Engine/VERTIX_ENGINE_EXPORT.h"

namespace Vertix::Engine {
    struct ModelLoadOptions {
        bool ApplyTransformationToModel = true;
        unsigned int AssimpPostProcessSteps =
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_FlipWindingOrder |
            aiProcess_FlipUVs |
            aiProcess_OptimizeMeshes;
    };

    struct ModelMaterialDeclaration {
        const aiMaterial* AssimpMaterial;
        std::string Name;
    };

    struct ModelMaterialLoadCallbackContext {
        std::vector<ModelMaterialDeclaration> Materials;
        std::vector<MaterialHandle> &MaterialHandles;
    };

    struct ModelLoadCallbackContext {
        Model* Model;
        std::string Name;
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
            const ModelLoadingContext* loadingContext,
            const aiNode* node,
            const aiScene* scene,
            const aiMatrix4x4t<float> &parentTransformation);

        static void ProcessMaterial(
            const aiScene* scene,
            ModelLoadingContext* loadingContext);

        static void ProcessMesh(
            const aiMesh* aiMesh,
            Mesh &mesh,
            const aiMatrix4x4t<float> &transformation,
            const ModelLoadingContext* loadingContext);
    };

    class ModelAsyncLoader {
        struct ModelLoadRequest {
            std::string FilePath;
            ModelLoadOptions Options;
            bool TryLoadMaterials;
            std::function<void(ModelHandle)> LoadedCallback;
        };

        struct ModelLoadingContext {
            ModelHandle Handle;
            Model* ModelPtr = nullptr;
        };

    public:
        VERTIX_ENGINE_API ModelAsyncLoader(
            ModelPool* modelPool,
            GraphicsDevice* graphicsDevice,
            const Microsoft::WRL::ComPtr<ID3D12CommandQueue> &copyCommandQueue,
            const Microsoft::WRL::ComPtr<ID3D12CommandQueue> &computeCommandQueue = nullptr,
            std::function<void(ModelMaterialLoadCallbackContext*)> materialLoadCallback = nullptr,
            const bool createRaytracingAccelerationStructure = false)
            : modelPool(modelPool), graphicsDevice(graphicsDevice), materialLoadCallback(std::move(materialLoadCallback)),
              copyCommandQueue(copyCommandQueue), computeCommandQueue(computeCommandQueue), d3d12Device(graphicsDevice->GetD3D12Device()),
              createRaytracingAccelerationStructure(createRaytracingAccelerationStructure) {}

        VERTIX_ENGINE_API void LoadModelAsync(
            const std::string &filePath,
            const ModelLoadOptions &options,
            const std::function<void(ModelHandle)> &modelLoadedCallback = nullptr,
            bool tryLoadMaterials = true);

        VERTIX_ENGINE_API void ExecuteAsync(
            DispatcherQueue* dispatcherQueue,
            std::function<void()> endCallback = nullptr);

    private:
        ModelPool* modelPool;
        GraphicsDevice* graphicsDevice;

        std::vector<ModelLoadRequest> modelLoadRequests;
        std::function<void(ModelMaterialLoadCallbackContext*)> materialLoadCallback;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> copyCommandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> computeCommandQueue;
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;

        bool createRaytracingAccelerationStructure = false;
    };
}

#endif //VERTIX_MODELIMPORTER_H
