//
// Created by Natsurainko on 2026/5/28.
//

#include "Vertix/Windowing/SwapChainBuffer.h"

#include "Vertix/Windowing/SwapChain.h"

Vertix::SwapChainBuffer::SwapChainBuffer(
    const SwapChain* swapChain,
    const uint32_t   frameIndex)
: RenderResource(
      RenderResourceKind::Buffer,
      GetBufferResource(swapChain, frameIndex),
      D3D12_RESOURCE_STATE_PRESENT
  ),
  swapChain(swapChain),
  frameIndex(frameIndex) {}

Microsoft::WRL::ComPtr<ID3D12Resource> Vertix::SwapChainBuffer::GetBufferResource(
    const SwapChain* swapChain,
    const uint32_t   frameIndex) {
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    ThrowIfFailed(swapChain->GetDXGISwapChain()->GetBuffer(frameIndex, IID_PPV_ARGS(&resource)));
    return resource;
}

void Vertix::SwapChainBuffer::RefreshBufferResource(const D3D12_RESOURCE_STATES resourceStates) {
    d3d12Resource        = GetBufferResource(swapChain, frameIndex);
    currentResourceState = resourceStates;
}
