//
// Created by Natsurainko on 2026/1/13.
//

#pragma once

#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/Graphics/ResourceUploadHeap.hpp"
#include "Vertix/Rendering/RenderResource.h"

namespace Vertix {
    class VertexBuffer : public RenderResource {
        D3D12_VERTEX_BUFFER_VIEW bufferView;
        uint32_t                 vertexCount;
        uint32_t                 strideSize;

    public:
        VERTIX_API VertexBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            uint32_t                                      vertexCount,
            uint32_t                                      strideSize);

        void IASetVertexBuffer(ID3D12GraphicsCommandList* commandList) const { commandList->IASetVertexBuffers(0, 1, &bufferView); }

        [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetBufferView() const noexcept { return bufferView; }
        [[nodiscard]] uint32_t                        GetVertexCount() const noexcept { return vertexCount; }
        [[nodiscard]] uint32_t                        GetStrideSize() const noexcept { return strideSize; }

        [[nodiscard]] VERTIX_API static std::unique_ptr<VertexBuffer> Create(
            const void*                data,
            uint32_t                   strideSize,
            uint32_t                   vertexCount,
            ID3D12Device*              device,
            ID3D12GraphicsCommandList* commandList,
            ResourceUploadHeap &       resourceUploadHeap);

        template <typename T>
        [[nodiscard]] static std::unique_ptr<VertexBuffer> Create(
            const std::vector<T> &     vector,
            ID3D12Device*              device,
            ID3D12GraphicsCommandList* commandList,
            ResourceUploadHeap &       resourceUploadHeap) {
            return Create(
                vector.data(),
                static_cast<uint32_t>(sizeof(T)),
                static_cast<uint32_t>(vector.size()),
                device,
                commandList,
                resourceUploadHeap
            );
        }

        template <typename T, uint32_t N>
        [[nodiscard]] static std::unique_ptr<VertexBuffer> Create(
            T                          data[N],
            ID3D12Device*              device,
            ID3D12GraphicsCommandList* commandList,
            ResourceUploadHeap &       resourceUploadHeap) {
            return Create(
                data,
                static_cast<uint32_t>(sizeof(T)),
                N,
                device,
                commandList,
                resourceUploadHeap
            );
        }
    };
}
