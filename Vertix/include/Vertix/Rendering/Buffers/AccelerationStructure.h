//
// Created by Natsurainko on 2026/3/28.
//

#pragma once

#include "Vertix/D3D12Interface.h"

#if VERTIX_D3D12_DEVICE_VERSION >= 5 && VERTIX_D3D12_COMMAND_LIST_VERSION >= 5

#include <memory>
#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/Graphics/ResourceUploadHeap.hpp"
#include "Vertix/Rendering/RenderBuffer.h"

namespace Vertix {
    class IndexBuffer;
    class VertexBuffer;

    class AccelerationStructure : public RenderBuffer {
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE type;
        Microsoft::WRL::ComPtr<ID3D12Resource>       scratchBuffer;

        AccelerationStructure(
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE  type,
            const Microsoft::WRL::ComPtr<ID3D12Resource> &resource,
            const Microsoft::WRL::ComPtr<ID3D12Resource> &scratchBuffer);

    public:
        [[nodiscard]] D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE GetType() const noexcept { return type; }
        [[nodiscard]] ID3D12Resource*                              GetScratchBuffer() const noexcept { return scratchBuffer.Get(); }

        [[nodiscard]] VERTIX_API static std::unique_ptr<AccelerationStructure> CreateBottomLevelAccelerationStructure(
            D3D12Interface::Device*      device,
            D3D12Interface::CommandList* commandList,
            const VertexBuffer*          vertexBuffer,
            const IndexBuffer*           indexBuffer,
            DXGI_FORMAT                  positionFormat);

        [[nodiscard]] VERTIX_API static std::unique_ptr<AccelerationStructure> CreateTopLevelAccelerationStructure(
            D3D12Interface::Device*                            device,
            D3D12Interface::CommandList*                       commandList,
            ResourceUploadHeap &                               resourceUploadHeap,
            const std::vector<D3D12_RAYTRACING_INSTANCE_DESC> &instanceDescs);
    };
}

#endif
