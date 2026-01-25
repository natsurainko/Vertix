//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_INDEXBUFFER_H
#define VERTIX_INDEXBUFFER_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

namespace Vertix {
    class IndexBuffer {
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        D3D12_INDEX_BUFFER_VIEW d3d12IndexBufferView;
        UINT indexCount;
    };
}

#endif //VERTIX_INDEXBUFFER_H