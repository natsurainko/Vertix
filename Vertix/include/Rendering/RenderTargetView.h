//
// Created by Natsurainko on 2026/1/23.
//

#ifndef VERTIX_RENDERTARGETVIEW_H
#define VERTIX_RENDERTARGETVIEW_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Math/Vector2D.h"

namespace Vertix {
    class GraphicsDevice;
    class RenderTargetView {
    public:
        RenderTargetView(GraphicsDevice* graphicsDevice,
                                  const D3D12_RESOURCE_DESC &rtvResourceDesc,
                                  const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
                                  const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc = nullptr,
                                  const D3D12_CLEAR_VALUE &clearValue = { .Color = { 0.0f, 0.0f, 0.0f, 1.0f } });

        void Resize(const Vector2D<UINT> &size);

        [[nodiscard]]
        Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12Resource() const {
            return d3d12Resource;
        }

        [[nodiscard]]
        D3D12_CPU_DESCRIPTOR_HANDLE GetHandle() const {
            return rtvHandle;
        }

    private:
        GraphicsDevice* graphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
        D3D12_RESOURCE_DESC rtvResourceDesc;
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        D3D12_CLEAR_VALUE clearValue;
        bool hasRtvDesc = false;
    };
}

#endif //VERTIX_RENDERTARGETVIEW_H