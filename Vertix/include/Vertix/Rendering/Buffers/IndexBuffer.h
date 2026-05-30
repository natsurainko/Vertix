//
// Created by Natsurainko on 2026/1/13.
//

#pragma once

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/Graphics/ResourceUploadHeap.hpp"
#include "Vertix/Rendering/RenderResource.h"

namespace Vertix {
    class IndexBuffer : public RenderResource {
        D3D12_INDEX_BUFFER_VIEW bufferView;
        DXGI_FORMAT             format;
        uint32_t                indexCount;

    public:
        VERTIX_API IndexBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            uint32_t                                      indexCount,
            DXGI_FORMAT                                   format);

        void IASetIndexBuffer(ID3D12GraphicsCommandList* commandList) const { commandList->IASetIndexBuffer(&bufferView); }

        [[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetBufferView() const noexcept { return bufferView; }
        [[nodiscard]] uint32_t                       GetIndexCount() const noexcept { return indexCount; }
        [[nodiscard]] DXGI_FORMAT                    GetFormat() const noexcept { return format; }

        [[nodiscard]] VERTIX_API static std::unique_ptr<IndexBuffer> Create(
            const void*                data,
            uint32_t                   indexCount,
            DXGI_FORMAT                format,
            ID3D12Device*              device,
            ID3D12GraphicsCommandList* commandList,
            ResourceUploadHeap &       resourceUploadHeap);

        [[nodiscard]] static std::unique_ptr<IndexBuffer> Create(
            const std::vector<uint32_t> &vector,
            ID3D12Device*                device,
            ID3D12GraphicsCommandList*   commandList,
            ResourceUploadHeap &         resourceUploadHeap) {
            return Create(
                vector.data(),
                static_cast<uint32_t>(vector.size()),
                DXGI_FORMAT_R32_UINT,
                device,
                commandList,
                resourceUploadHeap
            );
        }

        [[nodiscard]] static std::unique_ptr<IndexBuffer> Create(
            const std::vector<uint16_t> &vector,
            ID3D12Device*                device,
            ID3D12GraphicsCommandList*   commandList,
            ResourceUploadHeap &         resourceUploadHeap) {
            return Create(
                vector.data(),
                static_cast<uint32_t>(vector.size()),
                DXGI_FORMAT_R16_UINT,
                device,
                commandList,
                resourceUploadHeap
            );
        }

    private:
        static uint32_t GetStrideSize(DXGI_FORMAT format);
    };
}
