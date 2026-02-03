//
// Created by Natsurainko on 2026/1/28.
//

#ifndef VERTIX_SCENEOBJECT3D_H
#define VERTIX_SCENEOBJECT3D_H

#include "GameObject3D.h"
#include "Primitive/Model.h"

namespace Vertix::Engine {
    class SceneObject3D : public GameObject3D {
    public:
        ~SceneObject3D() override;

        void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const override;

        [[nodiscard]]
        bool TryLoadModelFromFile(const std::string &filePath);

        void UploadModelToGPU(const GraphicsDevice* graphicsDevice,
                              const GraphicsCommandList* graphicsCommandList,
                              TempGraphicsResourceHeap<Microsoft::WRL::ComPtr<ID3D12Resource>> &tempResourceHeap) const;
    protected:
        Model* sceneModel = nullptr;
    };
}

#endif //VERTIX_SCENEOBJECT3D_H