//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_SWAPCHAIN_H
#define VERTIX_SWAPCHAIN_H

#include <dxgi1_4.h>
#include <vector>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Math/Vector2D.hpp"
#include "Vertix/Rendering/RenderTexture.hpp"
#include "Vertix/Rendering/RenderTextureViewAllocator.hpp"

namespace Vertix {
    class GraphicsDevice;
    class SwapChain {
    public:
        VERTIX_API SwapChain(
            const GraphicsDevice *graphicsDevice,
            const HWND &hwnd,
            const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc);

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
        RenderTexture<RenderTarget>* GetRenderTarget(const UINT index) const noexcept {
            return renderTargets[index].get();
        }

        [[nodiscard]]
        RenderTexture<RenderTarget>* GetCurrentFrameRenderTarget() const noexcept {
            return renderTargets[currentFrameIndex].get();
        }

    private:
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;

        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> dxgiSwapChain;

        std::vector<std::unique_ptr<RenderTexture<RenderTarget>>> renderTargets;

        UINT currentFrameIndex = 0;
        UINT presentFlags = 0;
        UINT presentSyncInterval = 1;

        bool enableVSync = true;
    };
}

#endif //VERTIX_SWAPCHAIN_H
