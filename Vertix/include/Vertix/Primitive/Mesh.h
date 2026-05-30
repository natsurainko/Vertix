//
// Created by Natsurainko on 2026/1/10.
//

#pragma once

#include <string>
#include <vector>
#include <d3d12/d3d12.h>

#include "Vertix/Graphics/ResourceUploadHeap.hpp"
#include "Vertix/Primitive/Material.h"
#include "Vertix/Primitive/Vertex.h"

namespace Vertix {
    class GraphicsDevice;

    class VertexBuffer;
    class IndexBuffer;
    class AccelerationStructure;

    class Mesh {
    public:
        std::string Name;

        std::vector<Vertex>   Vertices;
        std::vector<uint32_t> Indices;

        DirectX::BoundingBox    BoundingBox;
        DirectX::BoundingSphere BoundingSphere;

        std::unique_ptr<VertexBuffer>          VertexBuffer;
        std::unique_ptr<IndexBuffer>           IndexBuffer;
        std::unique_ptr<AccelerationStructure> AccelerationStructure;

        MaterialHandle Material;

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
