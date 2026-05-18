//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_CONSTANTBUFFER_H
#define VERTIX_CONSTANTBUFFER_H

#include <cassert>
#include <memory>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Graphics/GraphicsDevice.h"
#include "Vertix/Rendering/RenderBuffer.h"

namespace Vertix {
    class ConstantBufferBase : public RenderBuffer {
    public:
        explicit ConstantBufferBase(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState,
            const size_t dataSize)
        : RenderBuffer(d3d12Resource, currentResourceState), dataSize(dataSize)
        {
            const CD3DX12_RANGE readRange(0, 0);
            ThrowIfFailed(d3d12Resource->Map(0, &readRange,
                reinterpret_cast<void**>(&bufferDataBegin)));
        }

        ~ConstantBufferBase() override {
            if (bufferDataBegin) {
                d3d12Resource->Unmap(0, nullptr);
                bufferDataBegin = nullptr;
            }
        }

        static D3D12_RESOURCE_DESC DESC(const size_t dataSize) noexcept {
            return CD3DX12_RESOURCE_DESC::Buffer((dataSize + 255) & ~255);
        }

    protected:
        void FillRaw(const void* data, const size_t offset, const size_t size) const {
            assert(offset + size <= dataSize && "FillRaw out of bounds");
            memcpy(bufferDataBegin + offset, data, size);
        }

        size_t dataSize = 0;
    private:
        UINT8* bufferDataBegin = nullptr;
    };

    template <typename T>
    class ConstantBuffer : public ConstantBufferBase {
    public:
        explicit ConstantBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState)
        : ConstantBufferBase(d3d12Resource, currentResourceState, sizeof(T)) {}

        void Fill(const T& value) {
            FillRaw(&value, 0, sizeof(T));
        }

        template<typename TMember>
        void FillField(const TMember &value, TMember T::* field) {
            const T* dummy = nullptr;
            const size_t offset = reinterpret_cast<size_t>(&(dummy->*field));
            FillRaw(&value, offset, sizeof(TMember));
        }

        static D3D12_RESOURCE_DESC DESC() noexcept {
            return ConstantBufferBase::DESC(sizeof(T));
        }

        static std::unique_ptr<ConstantBuffer> Create(const GraphicsDevice* device) {
            Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
            const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
            const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) + 255 & ~255);
            ThrowIfFailed(device->GetD3D12Device()->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&d3d12Resource)
            ));

            return std::make_unique<ConstantBuffer>(d3d12Resource, D3D12_RESOURCE_STATE_GENERIC_READ);
        }
    };
}

#endif //VERTIX_CONSTANTBUFFER_H
