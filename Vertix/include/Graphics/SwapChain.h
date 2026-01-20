//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_SWAPCHAIN_H
#define VERTIX_SWAPCHAIN_H

#include <dxgi1_4.h>
#include <vector>
#include <wrl/client.h>

#include "d3d12/d3dx12_root_signature.h"
#include "Math/Vector2D.h"

namespace Vertix {
    class GraphicsDevice;
    class SwapChain {
    public:
        SwapChain(const GraphicsDevice *graphicsDevice, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc);

        [[nodiscard]] UINT GetCurrentFrameIndex() const;
        Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentFrameRenderTargetResource();
        [[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameRenderTargetHandle() const;

        void PresentFrame();
        void Resize(const Vector2D<UINT> &size);

        [[nodiscard]] Vector2D<UINT> GetFrameSize() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> dxgiSwapChain;
        UINT currentFrameIndex = 0;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets;

        UINT renderTargetsDescriptorLength = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleForHeapStart{};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetsDescriptorHeap;
    };
}

#endif //VERTIX_SWAPCHAIN_H