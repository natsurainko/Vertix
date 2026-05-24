//
// Created by Natsurainko on 2026/3/10.
//

#ifndef VERTIX_STRUCTUREDBUFFER_H
#define VERTIX_STRUCTUREDBUFFER_H

#include <cassert>
#include <memory>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Rendering/RenderBuffer.h"
#include "Vertix/Graphics/GraphicsDevice.h"

namespace Vertix {
    class StructuredBufferBase : public RenderBuffer {
    public:
        explicit StructuredBufferBase(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState,
            const Microsoft::WRL::ComPtr<ID3D12Resource>& stagingBuffer,
            const UINT elementCount,
            const size_t elementSize)
        : RenderBuffer(d3d12Resource, currentResourceState), stagingBuffer(stagingBuffer), elementCount(elementCount), elementSize(elementSize)
        {
            const CD3DX12_RANGE readRange(0, 0);
            ThrowIfFailed(stagingBuffer->Map(0, &readRange,
                reinterpret_cast<void**>(&stagingDataBegin)));
        }

        ~StructuredBufferBase() override {
            if (stagingDataBegin) {
                stagingBuffer->Unmap(0, nullptr);
                stagingDataBegin = nullptr;
            }
        }

        [[nodiscard]] UINT GetElementCount() const noexcept { return elementCount; }
        [[nodiscard]] size_t GetElementSize() const noexcept { return elementSize; }

    protected:
        void FillRaw(
            ID3D12GraphicsCommandList* cmdList,
            const UINT index,
            const void* data,
            const size_t offset,
            const size_t size)
        {
            assert(stagingDataBegin && "Invalid bufferDataPtr");
            assert(index < elementCount && "Index out of range");
            assert(offset + size <= elementSize && "FillRaw out of bounds");

            const UINT64 byteOffset = static_cast<UINT64>(index) * elementSize + offset;
            memcpy(stagingDataBegin + byteOffset, data, size);

            const auto preState = currentResourceState;
            Transition(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);
            {
                cmdList->CopyBufferRegion(
                    d3d12Resource.Get(),
                    byteOffset,
                    stagingBuffer.Get(),
                    byteOffset,
                    size
                );
            }
            Transition(cmdList, preState);
        }

        void FillRawRange(
            ID3D12GraphicsCommandList* cmdList,
            const UINT  startIndex,
            const UINT  count,
            const void* data)
        {
            assert(stagingDataBegin && "Staging buffer not mapped");
            assert(count > 0 && "Count must be > 0");
            assert(startIndex + count <= elementCount && "FillRawRange out of bounds");

            const UINT64 dstOffset  = static_cast<UINT64>(startIndex) * elementSize;
            const UINT64 totalBytes = static_cast<UINT64>(count) * elementSize;
            memcpy(stagingDataBegin + dstOffset, data, totalBytes);

            const auto preState = currentResourceState;
            Transition(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);
            {
                cmdList->CopyBufferRegion(
                   d3d12Resource.Get(),
                   dstOffset,
                   stagingBuffer.Get(),
                   dstOffset,
                   totalBytes
                );
            }
            Transition(cmdList, preState);
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;
        UINT   elementCount = 0;
        size_t elementSize  = 0;

    private:
        UINT8* stagingDataBegin = nullptr;
    };

    template <typename T>
    class StructuredBuffer : public StructuredBufferBase {
    public:
        explicit StructuredBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState,
            const Microsoft::WRL::ComPtr<ID3D12Resource>& stagingBuffer,
            const UINT elementCount)
        : StructuredBufferBase(d3d12Resource, currentResourceState, stagingBuffer, elementCount, sizeof(T)) {}

        void Fill(
            ID3D12GraphicsCommandList* cmdList,
            const UINT index,
            const T& value)
        {
            FillRaw(cmdList, index, &value, 0, sizeof(T));
        }

        void FillRange(
            ID3D12GraphicsCommandList* cmdList,
            const UINT startIndex,
            const UINT count,
            const T* data)
        {
            FillRawRange(cmdList, startIndex, count, data);
        }

        void FillRange(
            ID3D12GraphicsCommandList* cmdList,
            const UINT startIndex,
            const std::vector<T>& values)
        {
            assert(!values.empty());
            FillRawRange(
                cmdList,
                startIndex,
                static_cast<UINT>(values.size()),
                values.data()
            );
        }

        static D3D12_RESOURCE_DESC DESC(const UINT elementCount) noexcept { return CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) * elementCount); }
        static std::unique_ptr<StructuredBuffer> Create(
            const GraphicsDevice* device,
            const uint32_t elementCount)
        {
            const UINT64 bufferSize = sizeof(T) * elementCount;
            const auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
            Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
            Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;

            {
                const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
                ThrowIfFailed(device->GetD3D12Device()->CreateCommittedResource(
                    &heapProps,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&d3d12Resource)));
            }

            {
                const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
                ThrowIfFailed(device->GetD3D12Device()->CreateCommittedResource(
                    &heapProps,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&stagingBuffer)));
            }

            return std::make_unique<StructuredBuffer>(
                d3d12Resource,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                stagingBuffer,
                elementCount);
        }
    };
}

#endif //VERTIX_STRUCTUREDBUFFER_H
