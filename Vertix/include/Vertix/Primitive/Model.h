//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODEL_H
#define VERTIX_MODEL_H

#include <vector>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Primitive/Mesh.h"

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
    class Model {
    public:
        std::vector<Mesh> Meshes;
        ModelTransformation Transformation = {
            .Position = DirectX::SimpleMath::Vector3::Zero,
            .Scale = DirectX::SimpleMath::Vector3::One,
            .Orientation = DirectX::SimpleMath::Quaternion::Identity,
        };

        VERTIX_API void UploadToGPU(
            const Microsoft::WRL::ComPtr<ID3D12Device> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,
            ResourceUploadHeap &resourceUploadHeap);

        VERTIX_API void UploadBLASToGPU(
            const Microsoft::WRL::ComPtr<ID3D12Device5> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> &commandList);

        VERTIX_API void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList) const;

        VERTIX_API void DrawInstanced(
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
            UINT instanceCount) const;
    };
}

#endif //VERTIX_MODEL_H
