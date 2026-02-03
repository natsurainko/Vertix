//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_VERTEXBUFFER_H
#define VERTIX_VERTEXBUFFER_H

#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Graphics/GraphicsCommandList.h"
#include "Graphics/TempGraphicsResourceHeap.h"
#include "Primitive/Vertex.h"

namespace Vertix {
    class VertexBuffer {
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        D3D12_VERTEX_BUFFER_VIEW d3d12VertexBufferView;
        UINT vertexCount;

        static VertexBuffer* Create(const std::vector<Vertex> &vertices,
                                    const GraphicsDevice* graphicsDevice,
                                    const GraphicsCommandList* graphicsCommandList,
                                    TempGraphicsResourceHeap<Microsoft::WRL::ComPtr<ID3D12Resource>> &tempResourceHeap);
    };
}

#endif //VERTIX_VERTEXBUFFER_H