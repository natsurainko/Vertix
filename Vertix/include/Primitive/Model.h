//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODEL_H
#define VERTIX_MODEL_H

#include <vector>

#include "Mesh.h"
#include "VERTIX_EXPORT.h"

namespace Vertix {
    struct ModelTag {};
    using ModelHandle = ResourceHandle<ModelTag>;

    struct ModelTransformation {
        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Scale;
        DirectX::SimpleMath::Quaternion Orientation;
    };

    class GraphicsDevice;
    class GraphicsCommandList;
    class VERTIX_API Model {
    public:
        std::vector<Mesh> Meshes;
        ModelTransformation Transformation = {
            .Position = DirectX::SimpleMath::Vector3::Zero,
            .Scale = DirectX::SimpleMath::Vector3::One,
            .Orientation = DirectX::SimpleMath::Quaternion::Identity,
        };

        void UploadToGPU(
            const Microsoft::WRL::ComPtr<ID3D12Device> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,
            ResourceUploadHeap &resourceUploadHeap);

        void UploadBLASToGPU(
            const Microsoft::WRL::ComPtr<ID3D12Device5> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> &commandList);

        void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList) const;

        void DrawInstanced(
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
            UINT instanceCount) const;
    };
}

#endif //VERTIX_MODEL_H