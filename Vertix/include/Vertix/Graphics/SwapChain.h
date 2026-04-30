//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_SWAPCHAIN_H
#define VERTIX_SWAPCHAIN_H

#include <dxgi1_4.h>
#include <vector>
#include <d3d12/d3dx12_root_signature.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    class GraphicsDevice;
    class SwapChain {
    public:
        VERTIX_API SwapChain(
            const GraphicsDevice *graphicsDevice,
            const HWND &hwnd,
            const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc,
            const D3D12_RENDER_TARGET_VIEW_DESC* renderTargetDesc = nullptr);

        VERTIX_API void PresentFrame();
        VERTIX_API void Resize(const Vector2D<UINT> &size);
        VERTIX_API void SetEnableVSync(const bool &enable) noexcept;

        [[nodiscard]]
        bool GetEnableVsync() const noexcept {
            return enableVSync;
        }

        [[nodiscard]]
        UINT GetFrameCount() const noexcept {
            return swapChainDesc.BufferCount;
        }

        [[nodiscard]]
        Vector2D<UINT> GetFrameSize() const noexcept {
            return { swapChainDesc.Width, swapChainDesc.Height };
        }

        [[nodiscard]]
        UINT GetCurrentFrameIndex() const noexcept {
            return currentFrameIndex;
        }

        [[nodiscard]]
        DXGI_FORMAT GetRenderTargetFormat() const noexcept {
            if (hasRenderTargetDesc) {
                return renderTargetDesc.Format;
            }

            return swapChainDesc.Format;
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Resource>& GetCurrentFrameRenderTargetResource() const noexcept {
            return rtvResources[currentFrameIndex];
        }

        [[nodiscard]]
        const CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCurrentFrameRenderTargetHandle() const noexcept {
            return rtvHandles[currentFrameIndex];
        }

    private:
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        D3D12_RENDER_TARGET_VIEW_DESC renderTargetDesc{};
        std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> dxgiSwapChain;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> rtvResources;

        UINT currentFrameIndex = 0;
        UINT presentFlags = 0;
        UINT presentSyncInterval = 1;

        bool enableVSync = true;
        bool hasRenderTargetDesc = false;
    };
}

#endif //VERTIX_SWAPCHAIN_H
