//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_INDEXBUFFER_H
#define VERTIX_INDEXBUFFER_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Graphics/ResourceUploadHeap.hpp"

namespace Vertix {
    class IndexBuffer {
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        D3D12_INDEX_BUFFER_VIEW d3d12IndexBufferView;
        UINT indexCount;

        VERTIX_API static IndexBuffer* Create(
            const std::vector<UINT32> &indices,
            const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
            ResourceUploadHeap &resourceUploadHeap);
    };
}

#endif //VERTIX_INDEXBUFFER_H
