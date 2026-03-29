//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MESH_H
#define VERTIX_MESH_H

#include <string>
#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Material.h"
#include "Vertex.h"
#include "VERTIX_EXPORT.h"
#include "Graphics/Raytracing/BottomLevelAccelerationStructure.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"

namespace Vertix {
    class ResourceUploadHeap;
    class GraphicsDevice;
    class VERTIX_API Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<UINT32> Indices;
        std::string Name;

        VertexBuffer* VertexBuffer = nullptr;
        IndexBuffer* IndexBuffer = nullptr;
        BottomLevelAccelerationStructure* BLAS = nullptr;

        MaterialHandle Material;

        ~Mesh();

        void UploadToGPU(
            const Microsoft::WRL::ComPtr<ID3D12Device> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
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

#endif //VERTIX_MESH_H