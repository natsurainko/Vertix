//
// Created by Natsurainko on 2026/1/16.
//

#ifndef VERTIX_DEPTHSTENCILVIEW_H
#define VERTIX_DEPTHSTENCILVIEW_H

#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_barriers.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    class GraphicsDevice;
    class DepthStencilView {
    public:
        VERTIX_API DepthStencilView(
            const GraphicsDevice* graphicsDevice,
            const D3D12_RESOURCE_DESC &dsvResourceDesc,
            const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
            const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc = nullptr,
            const D3D12_CLEAR_VALUE &clearValue = { .DepthStencil = { 1.0f, 0 } });

        [[nodiscard]]
        VERTIX_API CD3DX12_RESOURCE_BARRIER CreateTransitionBarrier(
            D3D12_RESOURCE_STATES before,
            D3D12_RESOURCE_STATES after,
            UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE) const;

        VERTIX_API void CreateShaderResourceView(
            const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
            D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) const;

        VERTIX_API void Resize(const Vector2D<UINT> &size);

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Resource>& GetD3D12Resource() const noexcept {
            return d3d12Resource;
        }

        [[nodiscard]]
        const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle() const noexcept {
            return dsvHandle;
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device;

        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
        D3D12_RESOURCE_DESC dsvResourceDesc;
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        D3D12_CLEAR_VALUE clearValue;
        bool hasDsvDesc = false;
    };
}

#endif //VERTIX_DEPTHSTENCILVIEW_H
