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
    const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc,
    const D3D12_RENDER_TARGET_VIEW_DESC* renderTargetDesc) : swapChainDesc(swapChainDesc)
{
    if (renderTargetDesc) {
        this->renderTargetDesc = *renderTargetDesc;
        hasRenderTargetDesc = true;
    }

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
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = frameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap)));
    }

    {
        rtvResources = std::vector<ComPtr<ID3D12Resource>>(frameCount);
        const UINT renderTargetsDescriptorLength = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < frameCount; i++) {
            ThrowIfFailed(dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvResources[i])));
            d3d12Device->CreateRenderTargetView(rtvResources[i].Get(), renderTargetDesc, rtvHandle);
            rtvHandles.push_back(rtvHandle);
            rtvHandle.Offset(1, renderTargetsDescriptorLength);
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
        rtvResources[i].Reset();
    }

    ThrowIfFailed(dxgiSwapChain->ResizeBuffers(
        swapChainDesc.BufferCount,
        swapChainDesc.Width,
        swapChainDesc.Height,
        swapChainDesc.Format,
        swapChainDesc.Flags));

    for (UINT i = 0; i < swapChainDesc.BufferCount; i++) {
        ThrowIfFailed(dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvResources[i])));
        d3d12Device->CreateRenderTargetView(rtvResources[i].Get(), hasRenderTargetDesc ? &renderTargetDesc : nullptr, rtvHandles[i]);
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
