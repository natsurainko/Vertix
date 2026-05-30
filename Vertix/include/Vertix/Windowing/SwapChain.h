//
// Created by Natsurainko on 2025/12/23.
//

#pragma once

#include <vector>

#include "SwapChainBuffer.h"
#include "Vertix/D3D12Interface.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    class GraphicsDevice;

    class SwapChain {
        D3D12Interface::CommandQueue* commandQueue;

        DXGI_SWAP_CHAIN_DESC1                                 swapChainDesc;
        Microsoft::WRL::ComPtr<D3D12Interface::DXGISwapChain> dxgiSwapChain;
        std::vector<SwapChainBuffer>                          swapChainBuffers;

        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        HANDLE                              fenceEvent;
        std::vector<uint64_t>               fenceValues;

        uint32_t currentFrameIndex   = 0;
        uint32_t presentFlags        = 0;
        uint32_t presentSyncInterval = 1;

        bool enableVSync = true;

    public:
        VERTIX_API SwapChain(
            D3D12Interface::Device*                                      device,
            D3D12Interface::CommandQueue*                                commandQueue,
            const Microsoft::WRL::ComPtr<D3D12Interface::DXGISwapChain> &dxgiSwapChain);

        VERTIX_API void PresentFrame();
        VERTIX_API void Resize(const Vector2D<uint32_t> &size);
        VERTIX_API void SetEnableVSync(bool enable) noexcept;

        [[nodiscard]] bool               GetEnableVsync() const noexcept { return enableVSync; }
        [[nodiscard]] uint32_t           GetFrameCount() const noexcept { return swapChainDesc.BufferCount; }
        [[nodiscard]] Vector2D<uint32_t> GetFrameSize() const noexcept { return { swapChainDesc.Width, swapChainDesc.Height }; }

        [[nodiscard]] SwapChainBuffer* GetBuffer(const uint32_t index) noexcept { return &swapChainBuffers[index]; }
        [[nodiscard]] SwapChainBuffer* GetCurrentBuffer() noexcept { return &swapChainBuffers[currentFrameIndex]; }
        [[nodiscard]] uint32_t         GetCurrentFrameIndex() const noexcept { return currentFrameIndex; }

        [[nodiscard]] D3D12Interface::DXGISwapChain* GetDXGISwapChain() const noexcept { return dxgiSwapChain.Get(); }
    };
}
