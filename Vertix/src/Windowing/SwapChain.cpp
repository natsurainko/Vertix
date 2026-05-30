//
// Created by Natsurainko on 2025/12/23.
//

#include "Vertix/Windowing/SwapChain.h"

Vertix::SwapChain::SwapChain(
    D3D12Interface::Device*                                      device,
    D3D12Interface::CommandQueue*                                commandQueue,
    const Microsoft::WRL::ComPtr<D3D12Interface::DXGISwapChain> &dxgiSwapChain)
: commandQueue(commandQueue),
  dxgiSwapChain(dxgiSwapChain) {
    ThrowIfFailed(dxgiSwapChain->GetDesc1(&swapChainDesc));
    currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();

    swapChainBuffers.reserve(swapChainDesc.BufferCount);
    for (uint32_t frameIndex = 0; frameIndex < swapChainDesc.BufferCount; ++frameIndex) {
        swapChainBuffers.emplace_back(this, frameIndex);
    }

    fenceValues.reserve(swapChainDesc.BufferCount);
    ThrowIfFailed(device->CreateFence(fenceValues[currentFrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    fenceValues[currentFrameIndex]++;

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
}

void Vertix::SwapChain::PresentFrame() {
    ThrowIfFailed(dxgiSwapChain->Present(presentSyncInterval, presentFlags));

    // Schedule a Signal command in the queue.
    const uint64_t currentFenceValue = fenceValues[currentFrameIndex];
    ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue));

    // Update the frame index.
    currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (fence->GetCompletedValue() < fenceValues[currentFrameIndex]) {
        ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[currentFrameIndex], fenceEvent));
        WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    fenceValues[currentFrameIndex] = currentFenceValue + 1;
}

void Vertix::SwapChain::Resize(const Vector2D<UINT> &size) {
    swapChainDesc.Width  = (std::max)(size.X, static_cast<UINT>(1));
    swapChainDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    for (uint32_t frameIndex = 0; frameIndex < swapChainDesc.BufferCount; ++frameIndex) {
        swapChainBuffers[frameIndex].Reset();
    }

    ThrowIfFailed(
        dxgiSwapChain->ResizeBuffers(
            swapChainDesc.BufferCount,
            swapChainDesc.Width,
            swapChainDesc.Height,
            swapChainDesc.Format,
            swapChainDesc.Flags
        )
    );

    currentFrameIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
    for (uint32_t frameIndex = 0; frameIndex < swapChainDesc.BufferCount; ++frameIndex) {
        swapChainBuffers[frameIndex].RefreshBufferResource(D3D12_RESOURCE_STATE_PRESENT);
    }
}

void Vertix::SwapChain::SetEnableVSync(const bool enable) noexcept {
    enableVSync = enable;

    if (enable) {
        presentSyncInterval = 1;
        presentFlags        = 0;
    } else {
        presentSyncInterval = 0;
        presentFlags        = DXGI_PRESENT_ALLOW_TEARING;
    }
}
