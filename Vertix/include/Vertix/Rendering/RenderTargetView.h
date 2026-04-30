//
// Created by Natsurainko on 2026/1/23.
//

#ifndef VERTIX_RENDERTARGETVIEW_H
#define VERTIX_RENDERTARGETVIEW_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    class GraphicsDevice;
    class RenderTargetView {
    public:
        VERTIX_API RenderTargetView(
            const GraphicsDevice* graphicsDevice,
            const D3D12_RESOURCE_DESC &rtvResourceDesc,
            const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
            const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc = nullptr,
            const D3D12_CLEAR_VALUE &clearValue = { .Color = { 0.0f, 0.0f, 0.0f, 0.0f } });

        [[nodiscard]]
        VERTIX_API D3D12_RESOURCE_BARRIER CreateTransitionBarrier(
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
            return rtvHandle;
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device;

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
        D3D12_RESOURCE_DESC rtvResourceDesc;
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        D3D12_CLEAR_VALUE clearValue;
        bool hasRtvDesc = false;
    };
}

#endif //VERTIX_RENDERTARGETVIEW_H
