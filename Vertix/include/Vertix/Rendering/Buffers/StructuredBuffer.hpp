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
            const UINT elementCount,
            const size_t elementSize)
        : RenderBuffer(d3d12Resource, currentResourceState), elementCount(elementCount), elementSize(elementSize)
        {
            const CD3DX12_RANGE readRange(0, 0);
            ThrowIfFailed(d3d12Resource->Map(0, &readRange,
                reinterpret_cast<void**>(&bufferDataBegin)));
        }

        ~StructuredBufferBase() override {
            if (bufferDataBegin) {
                d3d12Resource->Unmap(0, nullptr);
                bufferDataBegin = nullptr;
            }
        }

        [[nodiscard]] UINT GetElementCount() const noexcept { return elementCount; }
        [[nodiscard]] size_t GetElementSize() const noexcept { return elementSize; }

    protected:
        void FillRaw(const UINT index, const void* data, const size_t offset, const size_t size) const {
            assert(bufferDataBegin && "Invalid bufferDataPtr");
            assert(index < elementCount && "Index out of range");
            assert(offset + size <= elementSize && "FillRaw out of bounds");
            memcpy(bufferDataBegin + index * elementSize + offset, data, size);
        }

        UINT   elementCount = 0;
        size_t elementSize  = 0;

    private:
        UINT8* bufferDataBegin = nullptr;
    };

    template <typename T>
    class StructuredBuffer : public StructuredBufferBase {
    public:
        explicit StructuredBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState,
            const UINT elementCount)
        : StructuredBufferBase(d3d12Resource, currentResourceState, elementCount, sizeof(T)) {}

        void Fill(const UINT index, const T &value) {
            FillRaw(index, &value, 0, sizeof(T));
        }

        static D3D12_RESOURCE_DESC DESC(const UINT elementCount) noexcept { return CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) * elementCount); }
        static std::unique_ptr<StructuredBuffer> Create(
            const GraphicsDevice* device,
            const uint32_t elementCount)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
            const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
            const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) * elementCount);
            ThrowIfFailed(device->GetD3D12Device()->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&d3d12Resource)
            ));

            return std::make_unique<StructuredBuffer>(d3d12Resource, D3D12_RESOURCE_STATE_GENERIC_READ, elementCount);
        }
    };
}

#endif //VERTIX_STRUCTUREDBUFFER_H
