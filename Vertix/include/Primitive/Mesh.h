//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MESH_H
#define VERTIX_MESH_H

#include <string>
#include <vector>
#include <wrl/client.h>

#include "Vertex.h"
#include "d3d12/d3d12.h"
#include "Graphics/TempGraphicsResourceHeap.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"

namespace Vertix {
    class GraphicsDevice;
    class Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<UINT32> Indices;
        std::string Name;

        VertexBuffer* VertexBuffer = nullptr;
        IndexBuffer* IndexBuffer = nullptr;

        ~Mesh();

        void UploadToGPU(const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            TempGraphicsResourceHeap<Microsoft::WRL::ComPtr<ID3D12Resource>> &tempResourceHeap);

        void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const;
    };
}

#endif //VERTIX_MESH_H