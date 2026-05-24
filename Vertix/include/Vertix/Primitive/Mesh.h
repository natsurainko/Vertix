//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MESH_H
#define VERTIX_MESH_H

#include <string>
#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Graphics/Buffers/IndexBuffer.h"
#include "Vertix/Graphics/Buffers/VertexBuffer.h"
#include "Vertix/Graphics/Raytracing/BottomLevelAccelerationStructure.h"
#include "Vertix/Primitive/Material.h"
#include "Vertix/Primitive/Vertex.h"

namespace Vertix {
    class ResourceUploadHeap;
    class GraphicsDevice;
    class Mesh {
    public:
        std::string Name;

        std::vector<Vertex> Vertices;
        std::vector<UINT32> Indices;

        DirectX::BoundingBox BoundingBox;
        DirectX::BoundingSphere BoundingSphere;

        VertexBuffer* VertexBuffer = nullptr;
        IndexBuffer* IndexBuffer = nullptr;
        BottomLevelAccelerationStructure* BLAS = nullptr;

        MaterialHandle Material;

        VERTIX_API ~Mesh();

        VERTIX_API void UploadToGPU(
            const Microsoft::WRL::ComPtr<ID3D12Device> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
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

#endif //VERTIX_MESH_H
