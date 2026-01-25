//
// Created by Natsurainko on 2026/1/23.
//

#ifndef VERTIX_RENDERTARGETVIEW_H
#define VERTIX_RENDERTARGETVIEW_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

namespace Vertix {
    class GraphicsDevice;
    class RenderTargetView {
    public:
        explicit RenderTargetView(GraphicsDevice& graphicsDevice);

    private:
        GraphicsDevice* graphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    };
}

#endif //VERTIX_RENDERTARGETVIEW_H