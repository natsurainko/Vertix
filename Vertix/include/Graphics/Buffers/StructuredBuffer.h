//
// Created by Natsurainko on 2026/3/10.
//

#ifndef VERTIX_STRUCTUREDBUFFER_H
#define VERTIX_STRUCTUREDBUFFER_H

#include <vector>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

namespace Vertix {
    template <typename T>
    class StructuredBuffer {
    public:
        explicit StructuredBuffer(const GraphicsDevice* graphicsDevice, const UINT elementCount = 32)
            : elementCount(elementCount) {
            const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
            const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) * elementCount);

            ThrowIfFailed(graphicsDevice->GetD3D12Device()->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&d3d12Resource)
            ));

            const CD3DX12_RANGE readRange(0, 0);
            ThrowIfFailed(d3d12Resource->Map(0, &readRange,
                reinterpret_cast<void**>(&bufferDataBegin)));

            gpuVirtualAddress = d3d12Resource->GetGPUVirtualAddress();
        }

        ~StructuredBuffer() {
            if (bufferDataBegin) {
                d3d12Resource->Unmap(0, nullptr);
                bufferDataBegin = nullptr;
            }
        }

        void FillAt(const UINT index, const T &value) {
            if (index >= elementCount) {
                throw std::out_of_range("Index out of range");
            }

            memcpy(bufferDataBegin + index * sizeof(T), &value, sizeof(T));
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Resource>& GetD3D12Resource() const {
            return d3d12Resource;
        }

        [[nodiscard]]
        D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const {
            return gpuVirtualAddress;
        }

    private:
        UINT elementCount;

        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress;
        UINT8* bufferDataBegin = nullptr;
    };
}

#endif //VERTIX_STRUCTUREDBUFFER_H