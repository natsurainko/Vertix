//
// Created by Natsurainko on 2025/12/23.
//

#include "Graphics/SwapChain.h"

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

using Microsoft::WRL::ComPtr;

Vertix::SwapChain::SwapChain(
    const GraphicsDevice* graphicsDevice,
    const HWND hwnd,
    const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc) {

    const UINT frameCount = swapChainDesc.BufferCount;
    const ComPtr<IDXGIFactory6> dxgiFactory = graphicsDevice->GetDxgiFactory();
    d3d12Device = graphicsDevice->GetD3D12Device();

    {
        ComPtr<IDXGISwapChain1> dxgiSwapChain1;
        ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            graphicsDevice->GetD3D12CommandQueue().Get(),
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

        ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&renderTargetsDescriptorHeap)));
        renderTargetsDescriptorLength = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        descriptorHandleForHeapStart = renderTargetsDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    }

    {
        renderTargets = std::vector<ComPtr<ID3D12Resource>>(frameCount);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHandleForHeapStart);

        for (UINT i = 0; i < frameCount; i++) {
            ThrowIfFailed(dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
            d3d12Device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, renderTargetsDescriptorLength);
        }
    }
}

ComPtr<ID3D12Resource> Vertix::SwapChain::GetCurrentFrameRenderTargetResource() {
    return renderTargets[currentFrameIndex];
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Vertix::SwapChain::GetCurrentFrameRenderTargetHandle() const {
    return {descriptorHandleForHeapStart, static_cast<int>(currentFrameIndex), renderTargetsDescriptorLength};
}

UINT Vertix::SwapChain::GetCurrentFrameIndex() const {
    return currentFrameIndex;
}

void Vertix::SwapChain::PresentFrame() {
    ThrowIfFailed(dxgiSwapChain->Present(1, 0));
    currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
}

void Vertix::SwapChain::Resize(const Vector2D<UINT> &size) {
    DXGI_SWAP_CHAIN_DESC desc = {};
    ThrowIfFailed(dxgiSwapChain->GetDesc(&desc));

    for (UINT i = 0; i < desc.BufferCount; i++) {
        renderTargets[i].Reset();
    }

    ThrowIfFailed(dxgiSwapChain->ResizeBuffers(
        desc.BufferCount,
        size.X,
        size.Y,
        desc.BufferDesc.Format,
        desc.Flags));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHandleForHeapStart);
    for (UINT i = 0; i < desc.BufferCount; i++) {
        ThrowIfFailed(dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
        d3d12Device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, renderTargetsDescriptorLength);
    }

    currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
}

Vertix::Vector2D<UINT> Vertix::SwapChain::GetFrameSize() const {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    ThrowIfFailed(dxgiSwapChain->GetDesc1(&swapChainDesc));
    return { swapChainDesc.Width, swapChainDesc.Height };
}
