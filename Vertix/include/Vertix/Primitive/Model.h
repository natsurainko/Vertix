//
// Created by Natsurainko on 2026/1/10.
//

#pragma once

#include <vector>

#include "Vertix/Primitive/Mesh.h"

namespace Vertix {
    struct ModelTag {};

    using ModelHandle = ResourceHandle<ModelTag>;

    struct ModelTransformation {
        DirectX::SimpleMath::Vector3    Position;
        DirectX::SimpleMath::Vector3    Scale;
        DirectX::SimpleMath::Quaternion Orientation;
    };

    class GraphicsDevice;
    class CommandList;

    class Model {
    public:
        std::vector<Mesh>   Meshes;
        ModelTransformation Transformation = {
            .Position = DirectX::SimpleMath::Vector3::Zero,
            .Scale = DirectX::SimpleMath::Vector3::One,
            .Orientation = DirectX::SimpleMath::Quaternion::Identity,
        };

        VERTIX_API void UploadToGPU(
            ID3D12Device*              device,
            ID3D12GraphicsCommandList* commandList,
            ResourceUploadHeap &       resourceUploadHeap);

#if VERTIX_D3D12_DEVICE_VERSION >= 5 && VERTIX_D3D12_COMMAND_LIST_VERSION >= 5
        VERTIX_API void UploadAccelerationStructureToGPU(
            D3D12Interface::Device*      device,
            D3D12Interface::CommandList* commandList);
#endif

        VERTIX_API void Draw(ID3D12GraphicsCommandList* commandList) const;

        VERTIX_API void DrawInstanced(
            ID3D12GraphicsCommandList* commandList,
            uint32_t                   instanceCount) const;
    };
}
