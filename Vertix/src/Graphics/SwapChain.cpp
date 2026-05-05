//
// Created by Natsurainko on 2025/12/23.
//

#include "Vertix/Graphics/SwapChain.h"

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Graphics/GraphicsDevice.h"

using Microsoft::WRL::ComPtr;

Vertix::SwapChain::SwapChain(
    const GraphicsDevice* graphicsDevice,
    const HWND &hwnd,
    const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc) : swapChainDesc(swapChainDesc)
{
    const UINT frameCount = swapChainDesc.BufferCount;
    const ComPtr<IDXGIFactory6>& dxgiFactory = graphicsDevice->GetDxgiFactory();
    d3d12Device = graphicsDevice->GetD3D12Device();

    {
        ComPtr<IDXGISwapChain1> dxgiSwapChain1;
        ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            graphicsDevice->GetDefaultD3D12CommandQueue().Get(),
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &dxgiSwapChain1));

        ThrowIfFailed(dxgiSwapChain1.As(&dxgiSwapChain));
        currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
    }

    {
        for (UINT i = 0; i < frameCount; i++) {
            ComPtr<ID3D12Resource> resource;
            ThrowIfFailed(dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&resource)));
            renderTargets.push_back(std::make_unique<RenderTexture<RenderTarget>>(d3d12Device, resource, D3D12_RESOURCE_STATE_PRESENT));
        }
    }
}

void Vertix::SwapChain::PresentFrame() {
    ThrowIfFailed(dxgiSwapChain->Present(presentSyncInterval, presentFlags));
    currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
}

void Vertix::SwapChain::Resize(const Vector2D<UINT> &size) {
	swapChainDesc.Width = (std::max)(size.X, static_cast<UINT>(1));
	swapChainDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    for (UINT i = 0; i < swapChainDesc.BufferCount; i++) {
        renderTargets[i]->Reset();
    }

    ThrowIfFailed(dxgiSwapChain->ResizeBuffers(
        swapChainDesc.BufferCount,
        swapChainDesc.Width,
        swapChainDesc.Height,
        swapChainDesc.Format,
        swapChainDesc.Flags));

    for (UINT i = 0; i < swapChainDesc.BufferCount; i++) {
        ComPtr<ID3D12Resource> resource;
        ThrowIfFailed(dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&resource)));
        renderTargets[i]->Replace(resource);
    }

    currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
}

void Vertix::SwapChain::SetEnableVSync(const bool &enable) noexcept {
    enableVSync = enable;

    if (enable) {
        presentSyncInterval = 1;
        presentFlags = 0;
    } else {
        presentSyncInterval = 0;
        presentFlags = DXGI_PRESENT_ALLOW_TEARING;
    }
}
