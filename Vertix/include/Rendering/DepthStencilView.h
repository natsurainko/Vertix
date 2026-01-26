//
// Created by Natsurainko on 2026/1/16.
//

#ifndef VERTIX_DEPTHSTENCILVIEW_H
#define VERTIX_DEPTHSTENCILVIEW_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Math/Vector2D.h"

namespace Vertix {
    class GraphicsDevice;
    class DepthStencilView {
    public:
        DepthStencilView(GraphicsDevice* graphicsDevice,
                         const D3D12_RESOURCE_DESC &dsvResourceDesc,
                         const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
                         const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc = nullptr,
                         const D3D12_CLEAR_VALUE &clearValue = { .DepthStencil = { 1.0f, 0 } });

        void Resize(const Vector2D<UINT> &size);

        [[nodiscard]]
        Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12Resource() const {
            return d3d12Resource;
        }

        [[nodiscard]]
        D3D12_CPU_DESCRIPTOR_HANDLE GetHandle() const {
            return dsvHandle;
        }
    private:
        GraphicsDevice* graphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;

        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
        D3D12_RESOURCE_DESC dsvResourceDesc;
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
        D3D12_CLEAR_VALUE clearValue;
        bool hasDsvDesc = false;
    };
}

#endif //VERTIX_DEPTHSTENCILVIEW_H