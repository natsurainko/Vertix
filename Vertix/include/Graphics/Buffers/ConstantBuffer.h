//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_CONSTANTBUFFER_H
#define VERTIX_CONSTANTBUFFER_H

#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

namespace Vertix {
    class GraphicsDevice;
    template <typename T>
    class ConstantBuffer {
    public:
        explicit ConstantBuffer(const GraphicsDevice* graphicsDevice) {
            const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
            const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
                (sizeof(T) + 255) & ~255
            );

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
        }

        ~ConstantBuffer() {
            if (bufferDataBegin) {
                d3d12Resource->Unmap(0, nullptr);
                bufferDataBegin = nullptr;
            }
        }

        void Fill(const T& value) {
            memcpy(bufferDataBegin, &value, sizeof(T));
        }

        [[nodiscard]]
        Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12Resource() const {
            return d3d12Resource;
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        UINT8* bufferDataBegin = nullptr;
    };
}

#endif //VERTIX_CONSTANTBUFFER_H