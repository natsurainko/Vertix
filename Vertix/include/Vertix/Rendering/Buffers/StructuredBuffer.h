//
// Created by Natsurainko on 2026/3/10.
//

#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <d3d12/d3dx12.h>
#include <wrl/client.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Rendering/RenderBuffer.h"

namespace Vertix {
    class StructuredBufferBase : public RenderBuffer {
        uint8_t* stagingDataBegin = nullptr;

    protected:
        Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;
        uint32_t                               elementCount = 0;
        uint32_t                               elementSize  = 0;

    public:
        VERTIX_API explicit StructuredBufferBase(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            D3D12_RESOURCE_STATES                         currentResourceState,
            const Microsoft::WRL::ComPtr<ID3D12Resource> &stagingBuffer,
            uint32_t                                      elementCount,
            uint32_t                                      elementSize);

        VERTIX_API ~StructuredBufferBase() override;

        [[nodiscard]] uint32_t GetElementCount() const noexcept { return elementCount; }
        [[nodiscard]] uint32_t GetElementSize() const noexcept { return elementSize; }

    protected:
        VERTIX_API void FillRaw(
            ID3D12GraphicsCommandList* cmdList,
            uint32_t                   index,
            const void*                data,
            uint32_t                   offset,
            uint32_t                   size);

        VERTIX_API void FillRawRange(
            ID3D12GraphicsCommandList* cmdList,
            uint32_t                   startIndex,
            uint32_t                   count,
            const void*                data);
    };

    template <typename T>
    class StructuredBuffer : public StructuredBufferBase {
    public:
        explicit StructuredBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES                   currentResourceState,
            const Microsoft::WRL::ComPtr<ID3D12Resource> &stagingBuffer,
            const uint32_t                                elementCount)
        : StructuredBufferBase(d3d12Resource, currentResourceState, stagingBuffer, elementCount, sizeof(T)) {}

        void Fill(
            ID3D12GraphicsCommandList* cmdList,
            const uint32_t             index,
            const T &                  value) {
            FillRaw(cmdList, index, &value, 0, sizeof(T));
        }

        void FillRange(
            ID3D12GraphicsCommandList* cmdList,
            const uint32_t             startIndex,
            const uint32_t             count,
            const T*                   data) {
            FillRawRange(cmdList, startIndex, count, data);
        }

        void FillRange(
            ID3D12GraphicsCommandList* cmdList,
            const uint32_t             startIndex,
            const std::vector<T> &     values) {
            assert(!values.empty());
            FillRawRange(
                cmdList,
                startIndex,
                static_cast<UINT>(values.size()),
                values.data()
            );
        }

        static D3D12_RESOURCE_DESC DESC(const uint32_t elementCount) noexcept { return CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) * elementCount); }

        static std::unique_ptr<StructuredBuffer> Create(
            ID3D12Device*  device,
            const uint32_t elementCount) {
            Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
            Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;

            const uint64_t                bufferSize = sizeof(T) * elementCount;
            const auto                    bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
            const CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);
            const CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);

            ThrowIfFailed(
                device->CreateCommittedResource(
                    &defaultHeap,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&d3d12Resource)
                )
            );

            ThrowIfFailed(
                device->CreateCommittedResource(
                    &uploadHeap,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&stagingBuffer)
                )
            );

            return std::make_unique<StructuredBuffer>(d3d12Resource, D3D12_RESOURCE_STATE_GENERIC_READ, stagingBuffer, elementCount);
        }
    };
}
