//
// Created by Natsurainko on 2025/12/24.
//

#include "Graphics/FrameCommandList.h"

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

using Microsoft::WRL::ComPtr;

Vertix::FrameCommandList::FrameCommandList(const GraphicsDevice* graphicsDevice, const UINT frameCount)
    : GraphicsCommandList(graphicsDevice->GetD3D12Device(), graphicsDevice->GetDefaultD3D12CommandQueue()), frameCount(frameCount), fenceValue(0) {

    if (frameCount == 0) throw std::exception("frameCount cannot be zero");

    {
        allocators = std::vector<ComPtr<ID3D12CommandAllocator>>(frameCount);
        allocators[0] = commandAllocator;

        for (UINT i = 1; i < frameCount ; ++i) {
            ThrowIfFailed(d3d12Device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&allocators[i])));
        }
    }
    {
        fenceValues.resize(frameCount);
        ThrowIfFailed(d3d12Device->CreateFence(
            fenceValues[0],
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&fence)));

        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEvent == nullptr) {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }
}

Vertix::FrameCommandList::~FrameCommandList() {
    for (UINT i = 0; i < frameCount; i++) {
        if (fence->GetCompletedValue() < fenceValues[i]) {
            ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[i], fenceEvent));
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }

    if (fenceEvent) {
        CloseHandle(fenceEvent);
    }
}

void Vertix::FrameCommandList::BeginCommand(const ComPtr<ID3D12PipelineState> &pipelineState) const {
    const auto allocator = allocators[currentFrameIndex];
    ThrowIfFailed(allocator->Reset());
    ThrowIfFailed(graphicsCommandList->Reset(allocator.Get(), pipelineState.Get()));
}

void Vertix::FrameCommandList::WaitAllFrames() const {
    for (UINT i = 0; i < frameCount; ++i) {
        if (fence->GetCompletedValue() < fenceValues[i]) {
            ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[i], fenceEvent));
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }
}

void Vertix::FrameCommandList::WaitPreviousFrame() const {
    if (const UINT64 currentFenceValue = fenceValues[currentFrameIndex]; fence->GetCompletedValue() < currentFenceValue) {
        ThrowIfFailed(fence->SetEventOnCompletion(currentFenceValue, fenceEvent));
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}

void Vertix::FrameCommandList::MoveToNextFrame() {
    ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValue));
    fenceValues[currentFrameIndex] = fenceValue;
    fenceValue++;

    currentFrameIndex = (currentFrameIndex + 1) % frameCount;
}