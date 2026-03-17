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
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "VERTIX_EXPORT.h"

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

        MaterialHandle Material;

        ~Mesh();

        void UploadToGPU(
            const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            ResourceUploadHeap &resourceUploadHeap);

        void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const;

        void DrawInstanced(
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            UINT instanceCount) const;
    };
}

#endif //VERTIX_MESH_H