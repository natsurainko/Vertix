//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_SWAPCHAIN_H
#define VERTIX_SWAPCHAIN_H

#include <dxgi1_4.h>
#include <vector>
#include <d3d12/d3dx12_root_signature.h>
#include <wrl/client.h>

#include "Math/Vector2D.h"

namespace Vertix {
    class GraphicsDevice;
    class SwapChain {
    public:
        SwapChain(const GraphicsDevice *graphicsDevice,
                  HWND hwnd,
                  const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc);

        void PresentFrame();
        void Resize(const Vector2D<UINT> &size);
        void SetEnableVSync(const bool &enable);

        [[nodiscard]]
        bool GetEnableVsync() const {
            return enableVSync;
        }

        [[nodiscard]]
        Vector2D<UINT> GetFrameSize() const {
            return frameSize;
        }

        [[nodiscard]]
        UINT GetCurrentFrameIndex() const {
            return currentFrameIndex;
        }

        [[nodiscard]]
        Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentFrameRenderTargetResource() const {
            return renderTargets[currentFrameIndex];
        }

        [[nodiscard]]
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameRenderTargetHandle() const {
            return {
                descriptorHandleForHeapStart,
                static_cast<int>(currentFrameIndex),
                renderTargetsDescriptorLength
            };
        }
    private:
        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleForHeapStart{};
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> dxgiSwapChain;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetsDescriptorHeap;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets;

        Vector2D<UINT> frameSize;
        UINT currentFrameIndex = 0;
        UINT presentFlags = 0;
        UINT presentSyncInterval = 1;
        UINT renderTargetsDescriptorLength = 0;
        bool enableVSync = true;
    };
}

#endif //VERTIX_SWAPCHAIN_H