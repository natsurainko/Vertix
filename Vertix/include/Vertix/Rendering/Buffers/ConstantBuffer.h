//
// Created by Natsurainko on 2026/1/13.
//

#pragma once

#include <memory>
#include <d3d12/d3dx12.h>
#include <wrl/client.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Rendering/RenderBuffer.h"

namespace Vertix {
    class ConstantBufferBase : public RenderBuffer {
    protected:
        uint8_t* bufferDataBegin = nullptr;
        uint32_t dataSize;

    public:
        VERTIX_API explicit ConstantBufferBase(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &resource,
            D3D12_RESOURCE_STATES                         currentResourceState,
            uint32_t                                      dataSize);

        VERTIX_API ~ConstantBufferBase() override;

    protected:
        VERTIX_API void FillRaw(
            const void* data,
            uint32_t    offset,
            uint32_t    size) const;
    };

    template <typename T>
    class ConstantBuffer : public ConstantBufferBase {
    public:
        explicit ConstantBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES                   currentResourceState)
        : ConstantBufferBase(d3d12Resource, currentResourceState, sizeof(T)) {}

        void             Fill(const T &value) { FillRaw(&value, 0, sizeof(T)); }
        [[nodiscard]] T* Data() { return reinterpret_cast<T*>(bufferDataBegin); }

        [[nodiscard]] static D3D12_RESOURCE_DESC DESC() noexcept { return CD3DX12_RESOURCE_DESC::Buffer((sizeof(T) + 255) & ~255); }

        [[nodiscard]] static std::unique_ptr<ConstantBuffer> Create(ID3D12Device* device) {
            Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
            const CD3DX12_HEAP_PROPERTIES          heapProps(D3D12_HEAP_TYPE_UPLOAD);
            const CD3DX12_RESOURCE_DESC            bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) + 255 & ~255);
            ThrowIfFailed(
                device->CreateCommittedResource(
                    &heapProps,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&d3d12Resource)
                )
            );

            return std::make_unique<ConstantBuffer>(d3d12Resource, D3D12_RESOURCE_STATE_GENERIC_READ);
        }
    };
}
