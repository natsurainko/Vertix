//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_VERTEXBUFFER_H
#define VERTIX_VERTEXBUFFER_H

#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "VERTIX_EXPORT.h"
#include "Graphics/GraphicsCommandList.h"
#include "Graphics/ResourceUploadHeap.hpp"
#include "Primitive/Vertex.h"

namespace Vertix {
    class VertexBuffer {
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        D3D12_VERTEX_BUFFER_VIEW d3d12VertexBufferView;
        UINT vertexCount;

        static VERTIX_API VertexBuffer* Create(
            const std::vector<Vertex> &vertices,
            const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
            ResourceUploadHeap &resourceUploadHeap);

        static VERTIX_API VertexBuffer* CreateFullScreenRect(
            const GraphicsDevice* graphicsDevice,
            const GraphicsCommandList* graphicsCommandList,
            ResourceUploadHeap &resourceUploadHeap);
    };
}

#endif //VERTIX_VERTEXBUFFER_H